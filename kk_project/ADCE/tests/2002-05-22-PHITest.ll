; RUN: /home/marko/Downloads/llvm-project/build/bin/opt -load-pass-plugin=/home/marko/Downloads/llvm-project/build/lib/LLVMkkADCE.so -passes=my-adce < %s


define i32 @test(i1 %C, i32 %A, i32 %B) {
; <label>:0
        br i1 %C, label %BB1, label %BB2

BB1:            ; preds = %0
        br label %BB2

BB2:            ; preds = %BB1, %0
        %R = phi i32 [ %A, %0 ], [ %B, %BB1 ]           ; <i32> [#uses=1]
        ret i32 %R
}

