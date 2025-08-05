  #include "llvm/Transforms/KK_InstCombine/MyInstCombine.h"
  #include "llvm/IR/Constants.h"
  #include "llvm/IR/Instructions.h"
  #include "llvm/IR/PatternMatch.h"
  #include "llvm/IR/PassManager.h"
  #include "llvm/Passes/PassBuilder.h"
  #include "llvm/Passes/PassPlugin.h"
  #include "llvm/Support/raw_ostream.h"

  using namespace llvm;
  using namespace llvm::PatternMatch;

  PreservedAnalyses KKInstCombinePass::run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;
    errs() << "POKRENULO SE!!" << "\n";
    for (auto &BB : F) {
      for (auto Inst = BB.begin(); Inst != BB.end(); ) {
        Instruction *I = &*Inst++;

        // X + 2, X + 3 => X + 5
        Value *AddX;
        ConstantInt *AddC1, *AddC2;
        Instruction *Add1;
        if (match(I, m_Add(m_Instruction(Add1), m_ConstantInt(AddC2)))) {
          if (match(Add1, m_Add(m_Value(AddX), m_ConstantInt(AddC1)))) {
            IRBuilder<> Builder(I);
            Constant *Sum = ConstantInt::get(AddC1->getType(), AddC1->getSExtValue() + AddC2->getSExtValue());
            Value *NewAdd = Builder.CreateAdd(AddX, Sum);
            I->replaceAllUsesWith(NewAdd);
            I->eraseFromParent();

            if (Add1->use_empty()) {
              Add1->eraseFromParent();
            }
            errs() << "KKInstCombine: Combined two adds into one at function: " << F.getName() << "\n";

            Changed = true;
            continue;
          }
        }

        // X + 0
        if (match(I, m_Add(m_Value(AddX), m_ConstantInt(AddC1))) && AddC1->isZero()) {
          I->replaceAllUsesWith(AddX);
          I->eraseFromParent();
          errs() << "KKInstCombine: Removed add with zero at function: " << F.getName() << "\n";
          Changed = true;
          continue;
        }
        // 0 + X
        if (match(I, m_Add(m_ConstantInt(AddC1), m_Value(AddX))) && AddC1->isZero()) {
          I->replaceAllUsesWith(AddX);
          I->eraseFromParent();
          errs() << "KKInstCombine: Removed add with zero at function: " << F.getName() << "\n";
          Changed = true;
          continue;
        }

        // X * 2, X* 3 => X * 6
        Value *MulX;
        ConstantInt *MulC1, *MulC2;
        Instruction *Mul1;
        if (match(I, m_Mul(m_Instruction(Mul1), m_ConstantInt(MulC2)))) {
          if (match(Mul1, m_Mul(m_Value(MulX), m_ConstantInt(MulC1)))) {

            IRBuilder<> Builder(I);
            Constant *NewConst = ConstantInt::get(MulC1->getType(), MulC1->getValue() * MulC2->getValue());
            Value *NewMul = Builder.CreateMul(MulX, NewConst);
            I->replaceAllUsesWith(NewMul);
            I->eraseFromParent();

            if (Mul1->use_empty()) {
              Mul1->eraseFromParent();
            }

            errs() << "KKInstCombine: Combined two muls into one at function: " << F.getName() << "\n";
            Changed = true;
            continue;
          }
        }

        // X * 1
        if (match(I, m_Mul(m_Value(MulX), m_ConstantInt(MulC1))) && MulC1->isOne()) {
          I->replaceAllUsesWith(MulX);
          I->eraseFromParent();
          errs() << "KKInstCombine: Removed mul with one at function: " << F.getName() << "\n";
          Changed = true;
          continue;
        }

        // 1 * X
        if (match(I, m_Mul(m_ConstantInt(MulC1), m_Value(MulX))) && MulC1->isOne()) {
          I->replaceAllUsesWith(MulX);
          I->eraseFromParent();
          errs() << "KKInstCombine: Removed mul with one at function: " << F.getName() << "\n";
          Changed = true;
          continue;
        }

      }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  // Plugin registration
  extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
      LLVM_PLUGIN_API_VERSION,
      "KKInstCombine",
      "v0.1",
      [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
          [](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {
            if (Name == "kk-instcombine") {
              FPM.addPass(KKInstCombinePass());
              return true;
            }
            return false;
          });
      }
    };
  }
