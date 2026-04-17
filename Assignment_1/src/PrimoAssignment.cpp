#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"

using namespace llvm;

namespace {


struct AlgebricIdentity : public PassInfoMixin<AlgebricIdentity> {

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
        
        bool Transformed = false;

        for(auto &BB : F){
            for(Instruction &instr : BB){
                
                unsigned opCode = instr.getOpcode();

                switch (opCode)
                {
                    case Instruction::Add:
                        // gestiamo la proprietà COMMUTATIVA
                        if(ConstantInt *C = dyn_cast<ConstantInt>(instr.getOperand(1))){
                            if(C->isZero()){ // allora il secondo operando è 0
                                // Sostituisco usi della istruzione con il primo operando
                                instr.replaceAllUsesWith(instr.getOperand(0));
                                Transformed = true;
                                break;
                            }
                        }

                        if(ConstantInt *C = dyn_cast<ConstantInt>(instr.getOperand(0))){
                            if(C->isZero()){
                                instr.replaceAllUsesWith(instr.getOperand(1));
                                Transformed = true;
                            }
                        }
                        break;
                    
                    case Instruction::Mul:
                        // gestiamo la proprietà COMMUTATIVA
                        if(ConstantInt *C = dyn_cast<ConstantInt>(instr.getOperand(1))){
                            if(C->isOne()){
                                instr.replaceAllUsesWith(instr.getOperand(0));
                                Transformed = true;
                                break;
                            }
                        }

                        if(ConstantInt *C = dyn_cast<ConstantInt>(instr.getOperand(0))){
                            if(C->isOne()){
                                instr.replaceAllUsesWith(instr.getOperand(1));
                                Transformed = true;
                            }
                        }
                        break;
                    
                    case Instruction::Sub: // controlliamo solo il secondo opernado (non commutativa)
                        if(ConstantInt *C = dyn_cast<ConstantInt>(instr.getOperand(1))){
                            if(C->isZero()){
                                instr.replaceAllUsesWith(instr.getOperand(0));
                                Transformed = true;
                                break;
                            }
                        }
                        break;
                    
                    case Instruction::SDiv: // controlliamo solo il secondo opernado (non commutativa)
                        if(ConstantInt *C = dyn_cast<ConstantInt>(instr.getOperand(1))){
                            if(C->isOne()){
                                instr.replaceAllUsesWith(instr.getOperand(0));
                                Transformed = true;
                                break;
                            }
                        }
                        break;
                     
                    default:
                        continue;
                }
            }
        }

        

        if (Transformed) {
            return PreservedAnalyses::none();
        }

        return PreservedAnalyses::all();
    }

    static bool isRequired() { return true; }
};

struct StrengthReduction : public PassInfoMixin<StrengthReduction> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM){
        bool Transformed = false;

        for(auto &BB : F){
            for(Instruction &instr : BB){

                unsigned opCode = instr.getOpcode();
                
                switch(opCode)
                {
                    case Instruction::Mul: {
                        Value *op1 = instr.getOperand(0);
                        Value *op2 = instr.getOperand(1);

                        // prepariamo i due contenitori => gestiamo la proprietà COMMUTATIVA
                        ConstantInt *C = nullptr;
                        Value *var = nullptr;
                        
                        // cerco la costante
                        if(ConstantInt *CI = dyn_cast<ConstantInt>(op1)){
                            C = CI;
                            var = op2;
                        }else if(ConstantInt *CI = dyn_cast<ConstantInt>(op2)){
                            C = CI;
                            var = op1;
                        }

                        if(!C){ // se non ho constanti continue con prossima istruzione
                            continue;
                        }

                        APInt intVal = C->getValue(); // trasformiamo la constantInt in un formato testabile (ad esempio tramite powerof2())

                        // utilizzo IRBuilder per generare dinamicamente le nuove istruzioni ottimizzate
                        // e iniettarle in modo sicuro prima dell'istruzione originale.
                        IRBuilder<> Builder(&instr); // punta a &instr

                        if(intVal.isPowerOf2()){ // la costante è potenza del 2 perfetta

                            uint64_t shiftCount = intVal.logBase2(); // numero di volte da shiftare
                            Constant *shiftConst = ConstantInt::get(C->getType(), shiftCount); // traduco in un formato iniettabile nella shift

                            Value *shiftIntr = Builder.CreateShl(var, shiftConst); // costruzione shift ed inserimento PRIMA dell'istruzione originale
                            instr.replaceAllUsesWith(shiftIntr);
                            
                            Transformed = true;
                        }
                        else if((intVal+1).isPowerOf2()){ // se la costante + 1 è potenza del 2 (ovvero pot2 - 1 = costante)
                            uint64_t shiftCount = (intVal+1).logBase2();
                            Constant *shiftConst = ConstantInt::get(C->getType(), shiftCount);

                            Value *shiftIntr = Builder.CreateShl(var, shiftConst); // creazione shift 'standard'
                            Value *sub = Builder.CreateSub(shiftIntr, var); // sottrazione tra shift e valore di 'x' originale

                            instr.replaceAllUsesWith(sub); // costruzione ed inserimento della sub a seguito della shift

                            Transformed = true;
                        }
                        else if((intVal-1).isPowerOf2()){
                            uint64_t shiftCount = (intVal-1).logBase2();
                            Constant *shiftConst = ConstantInt::get(C->getType(), shiftCount);

                            Value *shiftIntr = Builder.CreateShl(var, shiftConst);
                            Value *add = Builder.CreateAdd(shiftIntr, var);

                            instr.replaceAllUsesWith(add);
                            
                            Transformed = true;
                        }


                        break;
                    }
                    
                    case Instruction::SDiv: {
                        Value *op1 = instr.getOperand(0);
                        Value *op2 = instr.getOperand(1);

                        ConstantInt *C = dyn_cast<ConstantInt>(op2); // controlliamo che il secondo operando sia una costante

                        if(!C){
                            continue;
                        }

                        APInt intVal = C->getValue();

                        if(intVal.isPowerOf2()){ // con la div trasformiamo solo se la constante è perfettamente una pot del 2

                            IRBuilder<> Builder(&instr);

                            uint64_t shiftCount = intVal.logBase2();
                            Constant *shiftConst = ConstantInt::get(C->getType(), shiftCount);
                            
                            //NOTA PROGETTUALE: ASSUNZIONE SUI NUMERI NEGATIVI
                            // Parto dal presupposto che il programma non debba operare su numeri 
                            // negativi dispari. Se così fosse, questa ottimizzazione sballerebbe di 1: 
                            // la divisione standard (SDiv) arrotonda verso lo zero (es. -3 / 2 = -1), 
                            // mentre lo shift aritmetico (AShr) arrotonda verso il basso (es. -3 >> 1 = -2).
                            Value *shiftIntr = Builder.CreateAShr(op1, shiftConst);

                            instr.replaceAllUsesWith(shiftIntr);

                            Transformed = true;

                        }
                        break;
                    }

                    default:
                        continue;
                }
            }
        }

        if (Transformed) {
            return PreservedAnalyses::none();
        }

        return PreservedAnalyses::all();
    }

    static bool isRequired() { return true; }
};

struct MultiInstruction : public PassInfoMixin<MultiInstruction> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM){
        bool Transformed = false;

        for(auto &BB : F){
            for(Instruction &instr : BB){

                unsigned opCode = instr.getOpcode();
                
                switch(opCode)
                {
                    case Instruction::Sub: {
                        Value *op1 = instr.getOperand(0);
                        Value *op2 = instr.getOperand(1);

                        ConstantInt *C_sub = dyn_cast<ConstantInt>(op2); // controllo se ho costante come secondo operando, sub non commutativa
                        if(!C_sub){
                            continue;
                        }

                        if(BinaryOperator *prevInstr = dyn_cast<BinaryOperator>(op1)){ // GUARDO INDIETRO: il primo operando è il risultato/dichiarazione di un operazione precedente?
                            if(prevInstr->getOpcode() == Instruction::Add){ // se era il ris di una add procedo 

                                ConstantInt *C_add = nullptr;
                                Value *var_add = nullptr; 

                                if(ConstantInt *CI = dyn_cast<ConstantInt>(prevInstr->getOperand(0))){
                                    C_add = CI;
                                    var_add = prevInstr->getOperand(1);
                                } else if(ConstantInt *CI = dyn_cast<ConstantInt>(prevInstr->getOperand(1))){
                                    C_add = CI;
                                    var_add = prevInstr->getOperand(0);
                                }
                                
                                if(C_add && C_sub->getValue() == C_add->getValue()){ // importante avere le costanti uguali
                                    // Istruzione PRECEDENTE (prevInstr): a = b + 1
                                    // Istruzione CORRENTE (instr):       c = a - 1
                                    // Instruzione SUCCESSIVA:            f = c
                                    // OBBIETTIVO: sostituire gli usi di 'c' direttamente con 'b' => f = b
                                    instr.replaceAllUsesWith(var_add);
                                    Transformed = true;
                                }
                            }
                        }
                        break;
                    }

                    case Instruction::Add: {
                        Value *op1 = instr.getOperand(0);
                        Value *op2 = instr.getOperand(1);

                        // Pattern standard per gestire la proprietà COMMUTATIVA dell'addizione
                        ConstantInt *C_add = nullptr;
                        Value *var = nullptr; 

                        // recupero la costante, se presente
                        if(ConstantInt *CI = dyn_cast<ConstantInt>(op1)){
                            C_add = CI;
                            var = op2;
                        } else if(ConstantInt *CI = dyn_cast<ConstantInt>(op2)){
                            C_add = CI;
                            var = op1;
                        }

                        if(!C_add){
                            continue; 
                        }

                        if(BinaryOperator *prevInstr = dyn_cast<BinaryOperator>(var)){
                            if(prevInstr->getOpcode() == Instruction::Sub){
                                if(ConstantInt *C_sub = dyn_cast<ConstantInt>(prevInstr->getOperand(1))){ // essendo una sub, per forza costante nel secondo operando
                                    if(C_add->getValue() == C_sub->getValue()){
                                        // Istruzione PRECEDENTE (prevInstr): a = b - 1
                                        // Istruzione CORRENTE (instr):       c = a + 1
                                        // OBBIETTIVO: sostituire gli usi di 'c' direttamente con 'b' -> c = b
                                        Value *var_sub = prevInstr->getOperand(0);
                                        instr.replaceAllUsesWith(var_sub);
                                        Transformed = true;
                                    }

                                }
                            }
                        }
                        break;

                    }
                    
                    case Instruction::Mul: {
                        Value *op1 = instr.getOperand(0);
                        Value *op2 = instr.getOperand(1);

                        // Pattern standard per gestire la proprietà COMMUTATIVA della mul
                        ConstantInt *C_mul = nullptr;
                        Value *var = nullptr;

                        if(ConstantInt *CI = dyn_cast<ConstantInt>(op1)){
                            C_mul = CI;
                            var = op2;
                        } else if(ConstantInt *CI = dyn_cast<ConstantInt>(op2)){
                            C_mul = CI;
                            var = op1;
                        }

                        if(!C_mul){
                            continue;
                        }

                        if(BinaryOperator *prevInstr = dyn_cast<BinaryOperator>(var)){
                            if(prevInstr->getOpcode() == Instruction::SDiv){
                                // CONTROLLO IMPORTANTE: se la divisione è perfetta e non ritorna resto, questo mi permette di non selezionare divisioni
                                // che mi farebbero perdere informazioni e che causerebbe un problema a catena nel programma.
                                if(prevInstr->isExact()){ 
                                    if(ConstantInt *C_sdiv = dyn_cast<ConstantInt>(prevInstr->getOperand(1))){ // ovviamente il secondo operando dato che la divisione non è COMMUTATIVA
                                        if(C_mul->getValue() == C_sdiv->getValue()){
                                            // Istruzione PRECEDENTE: a = sdiv exact b, K
                                            // Istruzione CORRENTE:   c = mul a, K   (o c = mul K, a)
                                            // OBIETTIVO: bypassare tutto e usare direttamente 'b'
                                            Value *var_sdiv = prevInstr->getOperand(0);
                                            instr.replaceAllUsesWith(var_sdiv);
                                            Transformed = true;
                                        }
                                    }
                                }
                            }
                        }
                        break;

                    }

                    case Instruction::SDiv: {
                        Value *op1 = instr.getOperand(0);
                        Value *op2 = instr.getOperand(1);
                        
                        // sdiv non commutativa, ovviamente costante come secondo operando, se presente
                        ConstantInt *C_div = dyn_cast<ConstantInt>(op2);
                        if(!C_div){
                            continue;
                        }

                        if(BinaryOperator *prevInstr = dyn_cast<BinaryOperator>(op1)){
                            if(prevInstr->getOpcode() == Instruction::Mul){
                                // Gestiamo la mul COMMUTATIVA
                                ConstantInt *C_mul = nullptr;
                                Value *var_mul = nullptr; // Questa sarà la nostra 'b' pulita

                                if(ConstantInt *CI = dyn_cast<ConstantInt>(prevInstr->getOperand(0))){
                                    C_mul = CI;
                                    var_mul = prevInstr->getOperand(1);
                                } else if(ConstantInt *CI = dyn_cast<ConstantInt>(prevInstr->getOperand(1))){
                                    C_mul = CI;
                                    var_mul = prevInstr->getOperand(0);
                                }

                                if(C_mul && C_div->getValue() == C_mul->getValue()){ // Se la mul ha la costante ed è uguale a quella della div (instr corrente)
                                    // Istruzione PRECEDENTE: a = mul b, K  (o a = mul K, b)
                                    // Istruzione CORRENTE:   c = sdiv a, K
                                    // OBIETTIVO: bypassare la divisione e rimpiazzarla con 'b' (var_mul)
                                    instr.replaceAllUsesWith(var_mul);
                                    Transformed = true;

                                }
                            }
                        }
                        break;
                    }

                    default:
                        break;
                }
            }
        } 

        if (Transformed) {
            return PreservedAnalyses::none();
        }

        return PreservedAnalyses::all();
    }

    static bool isRequired() { return true; }
};

}

llvm::PassPluginLibraryInfo getAlgebricIdentityPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, 
        "PrimoAssignment", 
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    
                    
                    if (Name == "algebric-identity") {
                        FPM.addPass(AlgebricIdentity());
                        return true;
                    }

                    if (Name == "strength-reduction") {
                        FPM.addPass(StrengthReduction());
                        return true;
                    }

                    if (Name == "multi-instruction") {
                        FPM.addPass(MultiInstruction());
                        return true;
                    }
                    return false;
                });
        }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getAlgebricIdentityPluginInfo();
}
