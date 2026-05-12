#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

// Fondamentale per i loop
#include "llvm/Analysis/LoopInfo.h" 

// Fondamentale per gestire i dominatori
#include "llvm/IR/Dominators.h"

// Per poter scorrere deminator tree tramite depth_first o breadth_first
#include "llvm/ADT/BreadthFirstIterator.h"

using namespace llvm;

namespace {

struct LoopPass : public PassInfoMixin<LoopPass> {

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        
        // oggetto LLVM che mappa tutti i "natural loops" del CFG e ci permette di lavorarci
        LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

        errs() << "Analizzando la funzione: " << F.getName() << "\n";

        if(LI.empty()){
            // se la funzione che sto analizzando non ha LOOP
            errs() << F.getName() << ": questa funzione non ha loop \n";
            return PreservedAnalyses::all(); // ritorno subito e passo alla prossima funzione
        }
        errs() << F.getName() << ": questa funzione contiene almeno un loop \n";
        
        // Se arrivo fino a qua presuppongo di star lavorando con un CFG con almeno un ciclo

        for(BasicBlock &BB : F){
            if(LI.isLoopHeader(&BB)){
                // Il BB è un header/SEP (single entry point) di un LOOP (naturale)
                errs() << "Trovato un BB header di un Loop: \n";
                errs() << "Blocco IR corrispondente: " << BB << "\n"; 
            }
        }

        int loopCounter = 0;

        // Itero sui loop trovati tramite LI
        // for(Loop *L : LI).
        //In LLVM, iterare in quel modo sull'oggetto LoopInfo ti restituisce 
        //esclusivamente i loop di livello zero (i "top-level loops") ignorando quindi
        // i loop annidati


        // per ottenere TUTTI loop (anche innestati):
        for(Loop *L : LI.getLoopsInPreorder()){
            loopCounter++;
            errs() << "Il loop numero " << loopCounter;

            // ovvero ha 1 preheader, 1 latch e exit blocks dedicati.
            if(L->isLoopSimplifyForm()){
                errs() << " e' in forma normale \n";
            }
            else{
                errs() << " non e' in forma normale \n";
            }

            // punto 3.b)
            // recuperiamo l'header del loop
            BasicBlock *Header = L->getHeader();

            if(Header){
                // prendo la fun cui header fa parte
                Function *parentFunction = Header->getParent();
                errs() << "Header del loop " << loopCounter << ": \n";
                errs() << Header << "\n";
                errs() << "Funzione recuperata dall'header: " << parentFunction->getName() << "\n";
                
                errs() << "--- Stampa CFG di " << parentFunction->getName() << " --- \n";
                parentFunction->print(errs()); // stampo il codice IR corrispondente al loop
                errs() << "\n";
            }

            // 3.c) Stampa dei blocchi del LOOP L:
            errs() << "Stampa blocchi appartenenti al " << loopCounter << " loop: \n";
            
            for(BasicBlock *BB : L->getBlocks()){
                // questo metodo restituisce sempre il loop più interno a cui appartiene un Basic Block.
                if(LI.getLoopFor(BB) == L){
                    errs() << *BB << "\n";
                }
            }
        }

        errs() << "===================================================================================== \n";

        return PreservedAnalyses::all();
    }
     
};

struct DomTreePass : public PassInfoMixin<DomTreePass> {

    // Funzione ricorsiva di supporto per stampare l'albero visivamente
    void printDomTree(DomTreeNode *Node, int level) {
        if (!Node){
            return;
        }

        // Aggiungo spazi in base al livello di profondità per l'indentazione
        for (int i = 0; i < level; ++i) {
            errs() << "    "; 
        }

        // Estraggo e stampo il Basic Block
        BasicBlock *BB = Node->getBlock(); // ottengo il BB a partire dalla strutture Node
        if (BB) {
            errs() << "|-- ";
            BB->printAsOperand(errs(), false); // Stampa il nome del blocco (es. %3)
            errs() << "\n";
        }

        // Itero sui figli diretti nell'albero dei dominatori
        for (DomTreeNode *Child : Node->children()) {
            printDomTree(Child, level + 1); // Scendo di un livello
        }
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        errs() << "\n=== Dominator Tree per la funzione: " << F.getName() << " ===\n";

        // Richiedo a LLVM di calcolare/fornirmi il Dominator Tree
        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
        // prendo la root
        DomTreeNode *Root = DT.getRootNode();

        if (Root) {
            printDomTree(Root, 0); // chiaramente parto dalla root
        } else {
            errs() << "Nessun albero dei dominatori trovato.\n";
        }
        
        errs() << "====================================================\n\n";

        return PreservedAnalyses::all();
    }
};

struct LoopInvariantCodeMotion : public PassInfoMixin<LoopInvariantCodeMotion> {

    bool isLoopInvariant(Instruction &I, Loop *L, std::vector<Instruction*> &LoopInvCandidates){

        // Ci concentriamo su operazioni matematica e bit a bit.
        // Operazioni che gestiscono flusso (branch) o memoria romperebbe la semantica del loop.
        if(!I.isBinaryOp() && !I.isShift() && !I.isCast()){
            return false;
        }

        // Ci concentriamo sul RHS dell'instruzione
        for(Use &Op : I.operands()){
            Value *Operand = Op.get();

            // Se operando è una costante (es: E = 4) allora è Loop-Invariant
            if(isa<Constant>(Operand)){
                continue;
            }

            // Se un operando è un'argomento della funzione, automaticamente possiamo constatare che la sua reaching
            // def è fuori dal loop, quindi è Loop-Invariant
            if(isa<Argument>(Operand)){
                continue;
            }

            // L'operando è il risultato di un istruzione precedente?
            if(Instruction *OpInstr = dyn_cast<Instruction>(Operand)){

                // Se il BB dell'instruzione per cui Operand è risultato, non appartiene al loop, allora la reaching def
                // arriva da fuori il loop, quindi è Loop-Invariant
                if(!L->contains(OpInstr->getParent())){
                    continue;
                }
                
                // Se arriviamo qua vuol dire che appartiene al loop, dobbiam controllare se la sua reaching def appartenente
                // al loop è già stata marcata come Loop-Invariant (è presente nel vettore 'LoopInvCandidates'), se è cosi 
                // allora anch'essa è loop-invariant, altrimenti no.
                if(std::find(LoopInvCandidates.begin(), LoopInvCandidates.end(), OpInstr) != LoopInvCandidates.end()){
                    continue;
                }

                return false;
            }
            
            return false; // Se arriviamo qua l'operando non rispetta nessuna condizione di invarianza, quidni non è Loop-invariant
        }

        return true; // Se arriviamo qua tuti gli operandi hanno rispettato le condizioni, quindi l'istruzione è Loop-Invariant
    }

    bool isSafeToMove(Instruction *I, DominatorTree &DT, Loop *L, std::string &Reason){

        BasicBlock *InstrBB = I->getParent();

        SmallVector<BasicBlock *, 8> ExitingBlocks;
        // riempiamo la struttura precedente con gli exiting block del loop
        L->getExitingBlocks(ExitingBlocks);

        for(BasicBlock *ExitBB : ExitingBlocks){
            // Verifica che il BB dell'instr candidata alla Code Motion domini TUTTE le uscite 
            if(!DT.dominates(InstrBB, ExitBB)){
                Reason = "Il BB dell'istruzione NON domina tutte le uscite";
                return false;
            }
        }


        // Dal momento che la IR è una SSA la condizione per la code motion riguardo all'unicità
        // della definizione di un LHS è già garantita


        for(User *U : I->users()){ // .users ci restiuisce tutti i 'Value' che usano il LHS di I
            // castiamo ad Instruction
            if(Instruction *UserInstr = dyn_cast<Instruction>(U)){
                BasicBlock *UserBlock = UserInstr->getParent();

                // ci interessano solo gli usi all'interno del loop
                if(L->contains(UserBlock)){
                    // se il BB dell'instr condidata alla Code Motion non domina i suoi usi non può essere spostata
                    if(!DT.dominates(InstrBB, UserBlock)){
                        Reason = "Il BB dell'istruzione NON domina un blocco che utilizza il suo LHS";
                        return false;
                    }
                }
            }

        }  
        return true;
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

        errs() << "-- Inizio LICM per la funzone: " << F.getName() << " --\n";

        for(Loop *L : LI.getLoopsInPreorder()){
            // è fondamentale che ogni loop sia in forma normale, fondamentale la presenza di un pre-header
            if(!L->isLoopSimplifyForm()){
                errs() << "-> Il loop " << L->getName() << " non è in forma normale, passo al prossimo! \n";
                continue;
            }

            // Struttura dati dove inserire instr Loop-Invariant
            std::vector<Instruction*> LoopInvCandidates; 
            bool changed;

            errs() << "-- Inizio fase di Loop-Invariant instruction check per il loop con Header: '";
            L->getHeader()->printAsOperand(errs(), false);
            errs() << "' ! --\n";

            // iteriamo finchè non troviamo nuove istruzioni loop-invariant
            do {
                changed = false;

                for (BasicBlock *BB : L->getBlocks()) {
                    for (Instruction &I : *BB) {
                        
                        // e l'abbiamo già inserita nei candidati, la saltiamo
                        if (std::find(LoopInvCandidates.begin(), LoopInvCandidates.end(), &I) != LoopInvCandidates.end()) {
                            continue;
                        }
                        if (isLoopInvariant(I, L, LoopInvCandidates)) {
                            errs() << "-> L'istruzione <" << I << " > è Loop-Invariant! \n";
                            LoopInvCandidates.push_back(&I);
                            changed = true; 
                        }
                    }
                }
            } while (changed);

            errs() << "-> Numero di istruzioni Loop-Invariant trovate: " << LoopInvCandidates.size() << "\n";

            // Inizio Code Motion:
            
            BasicBlock *Preheader = L->getLoopPreheader();
            // prendo ultima instr del preheader (tipicamente una branch all'header del loop)
            Instruction *PreheaderTerminator = Preheader->getTerminator();
            int movedCount = 0;

            errs() << "-- Inizio fase Code Motion per il loop con Header '";
            L->getHeader()->printAsOperand(errs(), false);
            errs() << "' ! --\n";

            for(Instruction *I : LoopInvCandidates){
                std::string Reason;
                if(isSafeToMove(I, DT, L, Reason)){
                    // possiamo procedere con lo spostamento, quindi la stacco dal BB di appartenenza
                    I->removeFromParent();
                    // Inseriamo prima del terminator del preheader
                    I->insertBefore(PreheaderTerminator);

                    errs() << "-> Spostamento istruzione <" << *I << "> nel preheader! \n";
                    movedCount++;
                }
                else{
                    errs() << "-> Impossibile spostare l'istruzione <" << *I << "> nel preheader! \n";
                    errs() << "-> Motivazione: " << Reason << " \n";
                }
            }

            errs() << "-> Istruzioni spostate nel PreHeader: " << movedCount << " \n";
        }
        return PreservedAnalyses::all();
    }
};

} 

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "LoopPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {

                    if (Name == "loop-pass") {
                        FPM.addPass(LoopPass());
                        return true;
                    }
                    if (Name == "dom-tree") {
                        FPM.addPass(DomTreePass());
                        return true;
                    }
                    if (Name == "loopInv-CodeMotion") {
                        FPM.addPass(LoopInvariantCodeMotion());
                        return true;
                    }
                    
                    return false;
                });
        }};
}