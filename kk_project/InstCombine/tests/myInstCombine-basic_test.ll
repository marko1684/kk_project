; RUN: /home/filip/Downloads/llvm-project/llvm-project/build/bin/opt -load-pass-plugin=/home/filip/Downloads/llvm-project/llvm-project/build/lib/KK_InstCombine.so -passes=kk-instcombine < %s

define i32 @basic_test(i32 %x) {
; CHECK: POKRENULO SE!!
; CHECK: KKInstCombine: Combined two adds into one at function: basic_test
entry:
  %add1 = add i32 %x, 5
  %add2 = add i32 %add1, 7
  ret i32 %add2
}
