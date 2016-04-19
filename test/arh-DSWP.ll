; ModuleID = 'arh-O0.ll'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%argstruct_1_ty = type { i32*, i32*, %outstruct_1_ty }
%outstruct_1_ty = type {}

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %res = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %retval
  store i32 0, i32* %res, align 4
  store i32 0, i32* %i, align 4
  br label %loop-replace

loop-replace:                                     ; preds = %entry
  %argstruct = alloca %argstruct_1_ty
  %i_argptr = getelementptr inbounds %argstruct_1_ty, %argstruct_1_ty* %argstruct, i64 0, i32 0
  store i32* %i, i32** %i_argptr
  %res_argptr = getelementptr inbounds %argstruct_1_ty, %argstruct_1_ty* %argstruct, i64 0, i32 1
  store i32* %res, i32** %res_argptr
  call void @sync_init()
  %argstruct_1_cast = bitcast %argstruct_1_ty* %argstruct to i8*
  call void @sync_delegate(i32 0, i8* (i8*)* @"1_subloop_0", i8* %argstruct_1_cast)
  call void @sync_delegate(i32 1, i8* (i8*)* @"1_subloop_1", i8* %argstruct_1_cast)
  call void @sync_join()
  br label %for.end

for.end:                                          ; preds = %loop-replace
  ret i32 0
}

declare void @sync_produce(i64, i32)

declare i64 @sync_consume(i32)

declare void @sync_join()

declare void @sync_init()

declare void @sync_delegate(i32, i8* (i8*)*, i8*)

declare void @showValue(i64)

declare void @showPlace()

declare void @showPtr(i8*)

define i8* @"1_subloop_0"(i8*) {
new-entry:
  call void @showPlace()
  %args = bitcast i8* %0 to %argstruct_1_ty*
  %i_arg = getelementptr inbounds %argstruct_1_ty, %argstruct_1_ty* %args, i64 0, i32 0
  %i_val = load i32*, i32** %i_arg, align 8
  %res_arg = getelementptr inbounds %argstruct_1_ty, %argstruct_1_ty* %args, i64 0, i32 1
  %res_val = load i32*, i32** %res_arg, align 8
  br label %for.cond_0

for.body_0:                                       ; preds = %for.cond_0
  store i32 999, i32* %res_val, align 4
  br label %for.inc_0

for.cond_0:                                       ; preds = %for.inc_0, %new-entry
  %t0_0 = load i32, i32* %i_val, align 4
  call void @sync_produce(i64 0, i32 0)
  call void @sync_produce(i64 0, i32 1)
  call void @sync_produce(i64 0, i32 2)
  call void @sync_produce(i64 0, i32 3)
  call void @sync_produce(i64 0, i32 4)
  br i1 undef, label %for.body_0, label %new-exit

for.inc_0:                                        ; preds = %for.body_0
  %t5_0 = load i32, i32* %i_val, align 4
  store i32 undef, i32* %i_val, align 4
  br label %for.cond_0

new-exit:                                         ; preds = %for.cond_0
  %load_outs = getelementptr inbounds %argstruct_1_ty, %argstruct_1_ty* %args, i64 0, i32 2
  ret i8* null
}

define i8* @"1_subloop_1"(i8*) {
new-entry:
  call void @showPlace()
  %args = bitcast i8* %0 to %argstruct_1_ty*
  %i_arg = getelementptr inbounds %argstruct_1_ty, %argstruct_1_ty* %args, i64 0, i32 0
  %i_val = load i32*, i32** %i_arg, align 8
  %res_arg = getelementptr inbounds %argstruct_1_ty, %argstruct_1_ty* %args, i64 0, i32 1
  %res_val = load i32*, i32** %res_arg, align 8
  br label %for.cond_1

for.body_1:                                       ; preds = %for.cond_1
  br label %for.inc_1

for.cond_1:                                       ; preds = %for.inc_1, %new-entry
  %t1_1 = icmp slt i32 undef, 10
  %c0 = call i64 @sync_consume(i32 0)
  %c1 = call i64 @sync_consume(i32 1)
  %c2 = call i64 @sync_consume(i32 2)
  %c3 = call i64 @sync_consume(i32 3)
  %c4 = call i64 @sync_consume(i32 4)
  br i1 %t1_1, label %for.body_1, label %new-exit

for.inc_1:                                        ; preds = %for.body_1
  %t6_1 = add nsw i32 undef, 1
  br label %for.cond_1

new-exit:                                         ; preds = %for.cond_1
  %load_outs = getelementptr inbounds %argstruct_1_ty, %argstruct_1_ty* %args, i64 0, i32 2
  ret i8* null
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.7.1 (tags/RELEASE_371/final)"}
