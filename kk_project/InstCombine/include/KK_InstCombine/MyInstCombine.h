#ifndef LLVM_TRANSFORMS_KK_INSTCOMBINE_H
#define LLVM_TRANSFORMS_KK_INSTCOMBINE_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class KKInstCombinePass : public PassInfoMixin<KKInstCombinePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_KK_INSTCOMBINE_H
