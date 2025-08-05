; RUN: /home/filip/Downloads/llvm-project/llvm-project/build/bin/opt -load-pass-plugin=/home/filip/Downloads/llvm-project/llvm-project/build/lib/KK_InstCombine.so -passes=kk-instcombine < %s

define i32 @add_zero_test(i32 %x) {
; CHECK: POKRENULO SE!!
; CHECK: KKInstCombine: Removed redundant add with zero at function: add_zero_test
entry:
  %a = add i32 %x, 0
  %b = add i32 0, %a
  ret i32 %b
}
