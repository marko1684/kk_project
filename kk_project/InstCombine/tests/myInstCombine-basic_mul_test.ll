; RUN: /home/filip/Downloads/llvm-project/llvm-project/build/bin/opt -load-pass-plugin=/home/filip/Downloads/llvm-project/llvm-project/build/lib/KK_InstCombine.so -passes=kk-instcombine < %s

define i32 @basic_mul_test(i32 %x) {
; CHECK: POKRENULO SE!!
; CHECK: KKInstCombine: Combined two muls into one at function: basic_test
entry:
  %a = mul i32 %x, 2
  %b = mul i32 %a, 3
  ret i32 %b
}
