; ModuleID = 'test-raw.ll'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_innestato_indipendente(ptr noalias noundef %0, ptr noalias noundef %1, i32 noundef %2, i32 noundef %3) #0 {
  br label %5 ; -> preheader loop Padre

5:                                                ; preds = %26, %4 -> header Loop Padre
  %.02 = phi i32 [ 0, %4 ], [ %27, %26 ]
  %6 = icmp slt i32 %.02, %3
  br i1 %6, label %7, label %28

7:                                                ; preds = %5 -> preheader L0
  br label %8

8:                                                ; preds = %14, %7 -> header L0
  %.01 = phi i32 [ 0, %7 ], [ %15, %14 ]
  %9 = icmp slt i32 %.01, %2
  br i1 %9, label %10, label %16

10:                                               ; preds = %8 -> body L0
  %11 = add nsw i32 %.02, %.01
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds i32, ptr %0, i64 %12
  store i32 %11, ptr %13, align 4
  br label %14

14:                                               ; preds = %10 -> latch L0
  %15 = add nsw i32 %.01, 1
  br label %8, !llvm.loop !6

16:                                               ; preds = %8 -> preheader L1 e exitBlock L0
  br label %17

17:                                               ; preds = %23, %16 -> header L1
  %.0 = phi i32 [ 0, %16 ], [ %24, %23 ]
  %18 = icmp slt i32 %.0, %2
  br i1 %18, label %19, label %25

19:                                               ; preds = %17 -> body L1
  %20 = mul nsw i32 %.02, 2
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds i32, ptr %1, i64 %21
  store i32 %20, ptr %22, align 4
  br label %23

23:                                               ; preds = %19 -> latch L1
  %24 = add nsw i32 %.0, 1
  br label %17, !llvm.loop !8

25:                                               ; preds = %17 -> exitBlock L1
  br label %26

26:                                               ; preds = %25 -> latch Loop Padre
  %27 = add nsw i32 %.02, 1
  br label %5, !llvm.loop !9

28:                                               ; preds = %5 -> exitBlock Loop Padre
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
!9 = distinct !{!9, !7}
