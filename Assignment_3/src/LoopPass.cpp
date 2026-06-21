#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Analysis/LoopInfo.h" 
#include "llvm/IR/Dominators.h"
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
        // In LLVM, iterare in quel modo sull'oggetto LoopInfo ti restituisce 
        // esclusivamente i loop di livello zero (i "top-level loops") ignorando quindi
        // i loop annidati


        // per ottenere TUTTI loop (anche innestati):
        // loop è una classe che rappresenta un natural loop
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

// Qui uso il DominatorTreeAnalysis per ottenere il dominator tree già 
// Calcolato dall'AM con DFS pre-order 
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

        BasicBlock *BB = Node->getBlock(); // ottengo il BB a partire dalla strutture Node
        if (BB) {
            errs() << "|-- ";
            BB->printAsOperand(errs(), false); 
            errs() << "\n";
        }

        for (DomTreeNode *Child : Node->children()) {
            printDomTree(Child, level + 1); // Scendo di un livello
        }
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        errs() << "\n=== Dominator Tree per la funzione: " << F.getName() << " ===\n";

        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
        DomTreeNode *Root = DT.getRootNode();

        if (Root) {
            printDomTree(Root, 0); 
        } else {
            errs() << "Nessun albero dei dominatori trovato.\n";
        }
        
        errs() << "====================================================\n\n";

        return PreservedAnalyses::all();
    }
};


// Obiettivo: identificare istruzioni il cui valore non cambia tra iterazioni
// (loop-invariant) e spostarle nel preheader, in modo che vengano eseguite
// una sola volta invece di N volte.
//
// L'algoritmo si articola in tre fasi:
//   1. isLoopInvariant  → determina se un'istruzione è candidata
//   2. isSafeToMove     → verifica che spostarla non alteri la semantica
//   3. Code Motion      → sposta fisicamente le istruzioni nel preheader
// =============================================================================
struct LoopInvariantCodeMotion : public PassInfoMixin<LoopInvariantCodeMotion> {

    // Il vettore 'LoopInvCandidates' accumula le istruzioni già identificate
    // come invarianti nei passaggi precedenti del do-while nel chiamante.
    bool isLoopInvariant(Instruction &I, Loop *L, std::vector<Instruction*> &LoopInvCandidates){

        // SCELTA CONSERVATIVA: limitiamo l'analisi a operazioni aritmetiche,
        // shift e cast. Escludiamo esplicitamente:
        //   - branch/terminatori  → gestiscono il control flow, non producono valori
        //   - load/store          → dipendono dalla memoria, che il loop può modificare
        //   - phi nodes           → per definizione aggregano valori di iterazioni diverse
        if(!I.isBinaryOp() && !I.isShift() && !I.isCast()){
            return false;
        }

        // Analizziamo ogni operando (RHS) dell'istruzione.
        // Basta che UN operando non sia invariant perché l'intera istruzione non lo sia.
        for(Use &Op : I.operands()){
            Value *Operand = Op.get();

            // Costante (es. il letterale '4')
            // Per definizione non dipende dall'iterazione corrente.
            if(isa<Constant>(Operand)){
                continue;
            }

            // Argomento della funzione: 
            // Viene fissato al momento della chiamata e non può essere
            // riscritto dal loop, quindi la sua reaching def è sempre esterna.
            if(isa<Argument>(Operand)){
                continue;
            }

            //Risultato di un'altra istruzione
            if(Instruction *OpInstr = dyn_cast<Instruction>(Operand)){

                // La definizione si trova in un BB fuori dal loop.
                // Poiché siamo in SSA, esiste una sola definizione: se è esterna
                // al loop il suo valore non può essere modificato dal loop stesso.
                if(!L->contains(OpInstr->getParent())){
                    continue;
                }

                // La definizione è dentro il loop ma è già stata
                // marcata loop-invariant in un'iterazione precedente del do-while.
                // Questo permette la propagazione transitiva:
                //   b = a*2  (invariant) → c = b+1 diventa invariant al giro dopo
                if(std::find(LoopInvCandidates.begin(), LoopInvCandidates.end(), OpInstr) != LoopInvCandidates.end()){
                    continue;
                }

                // La definizione è dentro il loop e non è ancora invariant:
                // l'operando dipende dall'iterazione, quindi l'istruzione non è invariant.
                return false;
            }

            // L'operando non rientra in nessuno dei casi sopra: conservativamente non invariant.
            return false;
        }

        // Tutti gli operandi hanno superato i controlli: l'istruzione è loop-invariant.
        return true;
    }

    // Essere loop-invariant NON è sufficiente per spostare un'istruzione.
    // Dobbiamo assicrarci che se lo spostiamo, non esegua mai in un certo percorso
    // quindi stare attenti alle condizioni all'interno del loop

    // Condizione necessaria: il BB dell'istruzione deve DOMINARE tutti i blocchi
    // di uscita del loop (exiting blocks).
    bool isSafeToMove(Instruction *I, DominatorTree &DT, Loop *L, std::string &Reason){

        BasicBlock *InstrBB = I->getParent();

        // Raccogliamo tutti i blocchi da cui il loop può uscire (hanno un successore fuori dal loop).
        SmallVector<BasicBlock *, 8> ExitingBlocks;
        L->getExitingBlocks(ExitingBlocks);

        for(BasicBlock *ExitBB : ExitingBlocks){
            // Il BB dell'istruzione deve dominare OGNI exiting block.
            // Se anche uno solo non è dominato, il movimento non è sicuro.
            if(!DT.dominates(InstrBB, ExitBB)){
                Reason = "Il BB dell'istruzione NON domina tutte le uscite";
                return false;
            }
        }

        // NOTA: in SSA la definizione di un valore è unica per costruzione,
        // Questo ci semplifica la vita e ci evita un controllo

        return true;
    }

    
    // Ultima fase
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

        errs() << "-- Inizio LICM per la funzone: " << F.getName() << " --\n";

        auto Loops = LI.getLoopsInPreorder();
        for(Loop *L : llvm::reverse(Loops)){

            // il loop deve essere in LoopSimplifyForm, ovvero avere:
            //   - un unico preheader (BB che precede l'header con un solo successore)
            //   - un unico latch    (BB che chiude il backedge verso l'header)
            //   - exit block dedicati
            // Senza preheader non avremmo un punto canonico dove inserire le istruzioni spostate.
            if(!L->isLoopSimplifyForm()){
                errs() << "-> Il loop " << L->getName() << " non è in forma normale, passo al prossimo! \n";
                continue;
            }

            // Vettore che accumula le istruzioni loop-invariant identificate.
            std::vector<Instruction*> LoopInvCandidates;
            bool changed;

            errs() << "-- Inizio fase di Loop-Invariant instruction check per il loop con Header: '";
            L->getHeader()->printAsOperand(errs(), false);
            errs() << "' ! --\n";

            // do-while iterativo invece di una singola passata lineare.
            // Motivazione: la proprietà loop-invariant si propaga per transitività.
            // Esempio:
            //   iterazione 1: b = a*2  → marcata invariant
            //   iterazione 2: c = b+1  → ora b è in LoopInvCandidates, quindi anche c è invariant
           // e quindi si procede fino a convergenza
            do {
                changed = false;

                for (BasicBlock *BB : L->getBlocks()) {
                    for (Instruction &I : *BB) {

                        // Evitiamo di riesaminare istruzioni già marcate invariant.
                        if (std::find(LoopInvCandidates.begin(), LoopInvCandidates.end(), &I) != LoopInvCandidates.end()) {
                            continue;
                        }
                        if (isLoopInvariant(I, L, LoopInvCandidates)) {
                            errs() << "-> L'istruzione <" << I << " > è Loop-Invariant! \n";
                            LoopInvCandidates.push_back(&I);
                            changed = true; // almeno una nuova invariant trovata: ripetiamo
                        }
                    }
                }
            } while (changed); // termina quando un'intera passata non produce nuove invarianti

            errs() << "-> Numero di istruzioni Loop-Invariant trovate: " << LoopInvCandidates.size() << "\n";

            // --- Code Motion ---

            BasicBlock *Preheader = L->getLoopPreheader();
        
            Instruction *PreheaderTerminator = Preheader->getTerminator();
            int movedCount = 0;

            errs() << "-- Inizio fase Code Motion per il loop con Header '";
            L->getHeader()->printAsOperand(errs(), false);
            errs() << "' ! --\n";

            for(Instruction *I : LoopInvCandidates){
                std::string Reason;
                if(isSafeToMove(I, DT, L, Reason)){
                    // removeFromParent: stacca l'istruzione dal suo BB originale
                    // senza distruggerla (i puntatori Use-Def rimangono validi).
                    I->removeFromParent();
                    // insertBefore: la reinserisce nel preheader immediatamente
                    // prima del branch terminatore.
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