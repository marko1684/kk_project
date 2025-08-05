#include "llvm/Transforms/kk_ADCEPass/kk_ADCEPass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/CFG.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

ADCEPass::ADCEPass() {
    errs() << "DEBUG ADCEPass constructor called 123 123\n";
}

void ADCEPass::markInitiallyLive(Function &F,
                                 SmallPtrSetImpl<Instruction *> &Live,
                                 SmallVectorImpl<Instruction *> &Worklist) {
    for (Instruction &I : instructions(F)) {
        if (I.isTerminator()) {
            Live.insert(&I);
            Worklist.push_back(&I);
            continue;
        }

        if (auto *SI = dyn_cast<StoreInst>(&I)) {
            if (SI->isVolatile()) {
                Live.insert(&I);
                Worklist.push_back(&I);
            }
            continue;
        }

        if (I.mayHaveSideEffects() || !I.use_empty()) {
            Live.insert(&I);
            Worklist.push_back(&I);
        }
    }
}


void ADCEPass::propagateLiveness(SmallPtrSetImpl<Instruction *> &Live,
                                 SmallVectorImpl<Instruction *> &Worklist) {
    while (!Worklist.empty()) {
        Instruction *I = Worklist.pop_back_val();
        for (Value *Op : I->operands()) {
            if (auto *OpInst = dyn_cast<Instruction>(Op)) {
                if (!Live.contains(OpInst)) {
                    Live.insert(OpInst);
                    Worklist.push_back(OpInst);
                }
            }
        }
    }
}

void ADCEPass::removeDeadInstructions(Function &F,
                                      const SmallPtrSetImpl<Instruction *> &Live,
                                      SmallVectorImpl<Instruction *> &ToErase) {
    for (Instruction &I : instructions(F)) {
        if (!Live.contains(&I)) {
            ToErase.push_back(&I);
        }
    }

    for (Instruction *I : llvm::reverse(ToErase)) {
        I->eraseFromParent();
    }
}

void ADCEPass::removeUnreachableBlocks(Function &F) {
    SmallPtrSet<BasicBlock *, 32> Reachable;
    SmallVector<BasicBlock *, 32> BBWorklist;

    BBWorklist.push_back(&F.getEntryBlock());

    while (!BBWorklist.empty()) {
        BasicBlock *BB = BBWorklist.pop_back_val();
        if (!Reachable.insert(BB).second)
            continue;
        for (BasicBlock *Succ : successors(BB)) {
            BBWorklist.push_back(Succ);
        }
    }

    SmallVector<BasicBlock *, 32> DeadBlocks;
    SmallPtrSet<BasicBlock *, 32> DeadSet;
    for (BasicBlock &BB : F) {
        if (!Reachable.contains(&BB)) {
            DeadBlocks.push_back(&BB);
            DeadSet.insert(&BB);
        }
    }

    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            if (auto *PN = dyn_cast<PHINode>(&I)) {
                for (unsigned i = 0; i < PN->getNumIncomingValues();) {
                    BasicBlock *Pred = PN->getIncomingBlock(i);
                    if (DeadSet.contains(Pred)) {
                        PN->removeIncomingValue(i, false); 
                    } else {
                        ++i;
                    }
                }
            }
        }
    }

    for (BasicBlock *BB : DeadBlocks) {
        for (Instruction &I : make_early_inc_range(*BB)) {
            I.dropAllReferences();
            I.eraseFromParent();
        }
    }

    for (BasicBlock *BB : DeadBlocks) {
        BB->eraseFromParent();
    }
}


PreservedAnalyses ADCEPass::run(Function &F, FunctionAnalysisManager &) {
    outs() << "Running ADCE on function: " << F.getName() << "\n";

    SmallPtrSet<Instruction *, 32> Live;
    SmallVector<Instruction *, 128> Worklist;
    SmallVector<Instruction *, 128> ToErase;

    markInitiallyLive(F, Live, Worklist);
    propagateLiveness(Live, Worklist);
    removeDeadInstructions(F, Live, ToErase);
    removeUnreachableBlocks(F);

    outs() << "ADCE removed " << ToErase.size() << " instructions\n";

    return PreservedAnalyses::none();
}

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "kkADCE", "v0.1",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "my-adce") {
                        FPM.addPass(ADCEPass());
                        return true;
                    }
                    return false;
                });
        }
    };
}
