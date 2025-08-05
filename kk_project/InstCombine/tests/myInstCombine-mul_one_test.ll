; RUN: /home/filip/Downloads/llvm-project/llvm-project/build/bin/opt -load-pass-plugin=/home/filip/Downloads/llvm-project/llvm-project/build/lib/KK_InstCombine.so -passes=kk-instcombine < %s

define i32 @mul_one_test(i32 %x) {
; CHECK: POKRENULO SE!!
; CHECK: KKInstCombine: Removed mul with one at function: test_mul_one
entry:
  %a = mul i32 %x, 1
  %b = mul i32 1, %a
  ret i32 %b
}
