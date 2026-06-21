#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/DependenceAnalysis.h"

#include "llvm/ADT/SmallVector.h"

using namespace llvm;

namespace {

struct LoopFusion : public PassInfoMixin<LoopFusion> {


    // CONDIZIONE 1) ADIACENZA
    bool areAdjacent(Loop *L0, Loop *L1){

        // Caso GUARDED
        if(L0->isGuarded() && L1->isGuarded()){

            BranchInst *G0 = L0->getLoopGuardBranch();
            BasicBlock *Preheader0 = L0->getLoopPreheader();

            BasicBlock *NonLoopSucc = nullptr;
            // individuo il BB al quale la branch nel guardBB di l0 salta se il loop non deve eseguire neanche una volta
            if(G0->getSuccessor(0) == Preheader0){
                NonLoopSucc = G0->getSuccessor(1);
            } else {
                NonLoopSucc = G0->getSuccessor(0);
            }

            BasicBlock *L1Guard = L1->getLoopGuardBranch()->getParent(); // individuo il BB di guardia del loop1
            return NonLoopSucc == L1Guard;
        }

        // Caso NON GUARDED
        BasicBlock *L0Exit = L0->getExitBlock();
        BasicBlock *L1Preheader = L1->getLoopPreheader();

        if (!L0Exit || !L1Preheader) {
            return false;
        }

        if (L0Exit == L1Preheader) return true;
        if (L0Exit->getSingleSuccessor() == L1Preheader) return true;

        return false;
    }

    // CONDIZIONE 2) STESSO NUMERO DI ITERAZIONI 
    bool sameTripCount(Loop *L0, Loop *L1, ScalarEvolution &SE){

        // getBackedgeTakenCount restituisce un'espressione SCEV.
        const SCEV *TripCount0 = SE.getBackedgeTakenCount(L0);
        const SCEV *TripCount1 = SE.getBackedgeTakenCount(L1);

        if(isa<SCEVCouldNotCompute>(TripCount0) || isa<SCEVCouldNotCompute>(TripCount1)){
            return false;
        }

        // Sfruttiamo l'Unique Folding
        return TripCount0 == TripCount1;
    }


    // CONDIZIONE 3) CONTROL FLOW EQUIVALENCE
    bool areControlFlowEquivalent(Loop *L0, Loop *L1, DominatorTree &DT, PostDominatorTree &PDT){
    
        // prendo il considerazione il caso Guarded. Se sono guarded devo analizzare l'equivalenza usando i blocchi di guardia dei loop
        BasicBlock *BlockToCheck0 = L0->isGuarded() ? L0->getLoopGuardBranch()->getParent() : L0->getHeader();
        BasicBlock *BlockToCheck1 = L1->isGuarded() ? L1->getLoopGuardBranch()->getParent() : L1->getHeader();

        bool dominates = DT.dominates(BlockToCheck0, BlockToCheck1);
        bool postDominates = PDT.dominates(BlockToCheck1, BlockToCheck0);

        return dominates && postDominates;
    }

    // CONDIZIONE 4) NESSUNA DIPENDENZA A DISTANZA NEGATIVA
    bool noNegativeDistanceDeps(Loop *L0, Loop *L1, DependenceInfo &DI){

        SmallVector<Instruction *> StoresL0;
        SmallVector<Instruction *> LoadsL1;

        // Raccolgo le store di L0
        for(BasicBlock *BB : L0->getBlocks()){
            for(Instruction &I : *BB){
                if(isa<StoreInst>(I)) StoresL0.push_back(&I);
            }
        }
        // Raccolgo le Load di L1
        for(BasicBlock *BB : L1->getBlocks()){
            for(Instruction &I : *BB){
                if(isa<LoadInst>(I)) LoadsL1.push_back(&I);
            }
        }

        for(Instruction *S : StoresL0){
            for(Instruction *Ld : LoadsL1){

                std::unique_ptr<Dependence> Dep = DI.depends(S, Ld, true);

                if(!Dep) continue; // nessuna dipendenza, posso fondere tranquillamente

                if (Dep->isConfused()) { // resto conservativo
                    errs() << "    [!] Aliasing ambiguo (isConfused). Abortito per sicurezza.\n";
                    return false;
                }

                unsigned Levels = Dep->getLevels();
                
                // approccio conservativo per loop livello 0 (loop top-level)
                if (Levels == 0) {
                    errs() << "    [!] Dipendenza a Livello 0 trovata. Fusione abortita in modo conservativo. \n";
                    return false;
                }
                
                // controllo dipendenza negativa per loop innestati
                for (unsigned lvl = 1; lvl <= Levels; ++lvl) {
                    unsigned Dir = Dep->getDirection(lvl);
                    if (Dir & Dependence::DVEntry::GT) { // se la dir è backward (greater then) allora non posso fondere i loop
                        errs() << "    [!] Dipendenza backward (distanza negativa) trovata.\n";
                        return false;
                    }
                }
            }
        }

        return true; // Nessuna dipendenza negativa trovata
    }

    // Funzioni helper
    BasicBlock *getBodyEntry(Loop *L){
        BranchInst *Br = cast<BranchInst>(L->getHeader()->getTerminator());
        for(unsigned i = 0; i < Br->getNumSuccessors(); ++i){
            if(L->contains(Br->getSuccessor(i))) return Br->getSuccessor(i);
        }
        return nullptr;
    }

    BasicBlock *getExitSuccessor(Loop *L){
        BranchInst *Br = cast<BranchInst>(L->getHeader()->getTerminator());
        for(unsigned i = 0; i < Br->getNumSuccessors(); ++i){
            if(!L->contains(Br->getSuccessor(i))) return Br->getSuccessor(i);
        }
        return nullptr;
    }

    // TRASFORMAZIONE: fondiamo L1 dentro L0
    bool fuseLoops(Loop *L0, Loop *L1){

        // Passo 1) Unifico le variabili di induzione
        PHINode *IV0 = L0->getCanonicalInductionVariable();
        PHINode *IV1 = L1->getCanonicalInductionVariable();
        if(!IV0 || !IV1){
            return false;
        }
        IV1->replaceAllUsesWith(IV0);

        // Passo 2) Recupero i BasicBlock chiave
        BasicBlock *Header0 = L0->getHeader();
        BasicBlock *Header1 = L1->getHeader();
        BasicBlock *Latch0  = L0->getLoopLatch();
        BasicBlock *Latch1  = L1->getLoopLatch();
        BasicBlock *ExitL1  = L1->getExitBlock();

        BasicBlock *Body0Entry = getBodyEntry(L0);   
        BasicBlock *Body1Entry = getBodyEntry(L1);   
        BasicBlock *ExitSucc0  = getExitSuccessor(L0);

        BasicBlock *Body0Last = Latch0 ? Latch0->getSinglePredecessor() : nullptr;
        BasicBlock *Body1Last = Latch1 ? Latch1->getSinglePredecessor() : nullptr;

        if(!Latch0 || !Latch1 || !ExitL1 || !Body0Entry || !Body1Entry ||
           !ExitSucc0 || !Body0Last || !Body1Last){
            return false;
        }

        // Gestiamo il caso guarded
        /*if (L0->isGuarded() && L1->isGuarded()) {
            BranchInst *G0 = L0->getLoopGuardBranch();
            BasicBlock *L1GuardBB = L1->getLoopGuardBranch()->getParent();
            
            // La branch della Guardia di L0 ha due destinazioni (Preheader0 o L1GuardBB).
            G0->replaceUsesOfWith(L1GuardBB, ExitL1);
        }*/

        // Passo 3) Ricucio i due loop (CFG Rewiring)
        
        // L'header di L0 esce verso l'USCITA di L1
        Header0->getTerminator()->replaceUsesOfWith(ExitSucc0, ExitL1);
        ExitL1->replacePhiUsesWith(Header1, Header0);

        // La fine del body di L0 va al BODY di L1
        Body0Last->getTerminator()->replaceUsesOfWith(Latch0, Body1Entry);
        Body1Entry->replacePhiUsesWith(Header1, Body0Last);

        // La fine del body di L1 va al LATCH di L0
        Body1Last->getTerminator()->replaceUsesOfWith(Latch1, Latch0);
        Latch0->replacePhiUsesWith(Body0Last, Body1Last);

        // Scollego il body dall'header di L1
        Header1->getTerminator()->replaceUsesOfWith(Body1Entry, Latch1);
        Latch1->replacePhiUsesWith(Body1Last, Header1);

        return true;
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {

        errs() << "\n=== LoopFusion sulla funzione: " << F.getName() << " ===\n";

        LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
        ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
        PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
        DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

        // analizzo tutti i loop innestati, non solo top-level
        SmallVector<Loop *> Loops;
        for (Loop *L : LI.getLoopsInPreorder()) {
            if (L->isInnermost()) { // se è un loop "fogliare", quindi senza un loop all'interno, lo considero per la loop fusion (conservativo)
                Loops.push_back(L);
            }
        }

        if(Loops.size() < 2){
            return PreservedAnalyses::all();
        }

        bool Changed = false;

        // analizzo ogni coppia (L[i], L[i+1])
        for(size_t i = 0; i + 1 < Loops.size(); ++i){
            Loop *L0 = Loops[i];
            Loop *L1 = Loops[i + 1];

            // Dal momento che loops non per forza contiene loop fratelli, controllo per sicurezza la fratellanza, ovvero se sono allo stesso livello.
            // se non sono fratelli abortisco in anticipo e risparmio computazionale
            // Se ho due loop top-level, questo if viene ignorato.
            if (L0->getParentLoop() != L1->getParentLoop()) {
                errs() << "    [!] L0 e L1 non sono fratelli (padri diversi). Salto.\n";
                continue; 
            }

            errs() << "-- Candidati: " << L0->getName() << " + " << L1->getName() << "\n";

            bool cond1 = areAdjacent(L0, L1);
            errs() << "  [1] Adiacenti? " << (cond1 ? "SI" : "NO") << "\n";

            bool cond2 = sameTripCount(L0, L1, SE);
            errs() << "  [2] Stesso numero iterazioni? " << (cond2 ? "SI" : "NO") << "\n";

            bool cond3 = areControlFlowEquivalent(L0, L1, DT, PDT);
            errs() << "  [3] Control flow equivalenti? " << (cond3 ? "SI" : "NO") << "\n";

            bool cond4 = false;
            // CONSIDERAZIONE: eseguo la DA solo dopo aver verificato positivamente i 3 vincoli precedenti per motivi
            // di performance, dato che la Dependence Analysis è costosa dal punto di vista computazione e per risorse
            if (cond1 && cond2 && cond3) {
                cond4 = noNegativeDistanceDeps(L0, L1, DI);
                errs() << "  [4] Nessuna dipendenza negativa? " << (cond4 ? "SI" : "NO") << "\n";
            }

            if(cond1 && cond2 && cond3 && cond4){
                errs() << "  => Tutte le condizioni OK: FUSIONE IN CORSO...\n";
                if(fuseLoops(L0, L1)){
                    Changed = true;
                    errs() << "FUSIONE COMPLETATA!\n";
                    break; // appena fondiamo terminiamo il passo
                }
            } else {
                errs() << "  => Fusione Abortita per la coppia corrente.\n";
            }
        }

        if(Changed){
            return PreservedAnalyses::none();
        }
        return PreservedAnalyses::all();
    }

    static bool isRequired() { return true; }
};

}

llvm::PassPluginLibraryInfo getLoopFusionPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "LoopFusion",
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "my-loop-fusion" || Name == "custom-loop-fusion") {
                        FPM.addPass(LoopFusion());
                        return true;
                    }
                    return false;
                });
        }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getLoopFusionPluginInfo();
}