#ifndef LLVM_TRANSFORMS_ADCEPASS_H
#define LLVM_TRANSFORMS_ADCEPASS_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class ADCEPass : public PassInfoMixin<ADCEPass> {
public:
    ADCEPass();
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

private:
    void markInitiallyLive(Function &F,
                           SmallPtrSetImpl<Instruction *> &Live,
                           SmallVectorImpl<Instruction *> &Worklist);

    void propagateLiveness(SmallPtrSetImpl<Instruction *> &Live,
                           SmallVectorImpl<Instruction *> &Worklist);

    void removeDeadInstructions(Function &F,
                                const SmallPtrSetImpl<Instruction *> &Live,
                                SmallVectorImpl<Instruction *> &ToErase);

    void removeUnreachableBlocks(Function &F);
};

} 
#endif
