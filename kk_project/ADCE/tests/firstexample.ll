; RUN: /home/marko/Downloads/llvm-project/build/bin/opt -load-pass-plugin=/home/marko/Downloads/llvm-project/build/lib/LLVMkkADCE.so -passes=my-adce < %s

define void @TestADCE (i32 %x, ptr %ptr) {
entry:
  %a = add i32 %x, 10            ; Dead (if not used)
  %b = mul i32 %a, 2             ; Dead
  %c = add i32 %b, %x            ; Dead
  %d = sub i32 %c, 1             ; Dead
  %cmp = icmp eq i32 %x, 0       ; Used in branch
  br i1 %cmp, label %if.then, label %if.else

if.then:
  br label %end

if.else:
  %dead1 = add i32 %x, 42        ; Dead
  %dead2 = mul i32 %dead1, 2     ; Dead
  store volatile i32 %x, ptr %ptr ; Volatile: must stay
  br label %end

end:
  ret void

unreachable_block:
  %u = add i32 1, 2              ; Dead block: whole block should be removed
  br label %end
}

; CHECK-LABEL: define void @TestADCE
; CHECK-NOT: %a = add
; CHECK-NOT: %b = mul
; CHECK-NOT: %c = add
; CHECK-NOT: %d = sub
; CHECK: store volatile i32 %x, ptr %ptr
; CHECK-NOT: %dead1
; CHECK-NOT: %dead2
; CHECK-NOT: %load = load i32, ptr %ptr
; CHECK: ret void
