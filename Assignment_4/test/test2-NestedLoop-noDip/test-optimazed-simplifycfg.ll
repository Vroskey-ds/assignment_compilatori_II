; ModuleID = 'test-optimazed.ll'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_innestato_indipendente(ptr noalias noundef %0, ptr noalias noundef %1, i32 noundef %2, i32 noundef %3) #0 {
  br label %5 ; -> preHeader Loop Padre

5:                                                ; preds = %17, %4 -> header Loop Padre
  %.02 = phi i32 [ 0, %4 ], [ %18, %17 ]
  %6 = icmp slt i32 %.02, %3
  br i1 %6, label %7, label %19

7:                                                ; preds = %5, %9 -> header LoopFuso
  %.01 = phi i32 [ %16, %9 ], [ 0, %5 ]
  %8 = icmp slt i32 %.01, %2
  br i1 %8, label %9, label %17

9:                                                ; preds = %7 -> body e latch LoopFuso
  %10 = add nsw i32 %.02, %.01
  %11 = sext i32 %.01 to i64
  %12 = getelementptr inbounds i32, ptr %0, i64 %11
  store i32 %10, ptr %12, align 4
  %13 = mul nsw i32 %.02, 2
  %14 = sext i32 %.01 to i64
  %15 = getelementptr inbounds i32, ptr %1, i64 %14
  store i32 %13, ptr %15, align 4

  %16 = add nsw i32 %.01, 1
  br label %7, !llvm.loop !6

17:                                               ; preds = %7 -> latch Loop Padre e exitBlock LoopFuso
  %18 = add nsw i32 %.02, 1
  br label %5, !llvm.loop !8

19:                                               ; preds = %5 -> exit Block Loop Padre
  ret void
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7 (/home/runner/work/llvm-project/llvm-project/clang cd708029e0b2869e80abe31ddb175f7c35361f90)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
