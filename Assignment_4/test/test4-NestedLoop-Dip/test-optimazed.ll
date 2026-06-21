; ModuleID = 'test-mem2reg-simplify.ll'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_confuso(ptr noalias noundef %0, i32 noundef %1, i32 noundef %2) #0 {
  br label %4

4:                                                ; preds = %26, %3
  %.03 = phi i32 [ 0, %3 ], [ %.1, %26 ]
  %.02 = phi i32 [ 0, %3 ], [ %27, %26 ]
  %5 = icmp slt i32 %.02, %2
  br i1 %5, label %6, label %28

6:                                                ; preds = %4
  br label %7

7:                                                ; preds = %13, %6
  %.01 = phi i32 [ 0, %6 ], [ %14, %13 ]
  %8 = icmp slt i32 %.01, %1
  br i1 %8, label %9, label %15

9:                                                ; preds = %7
  %10 = mul nsw i32 %.01, 10
  %11 = sext i32 %.01 to i64
  %12 = getelementptr inbounds i32, ptr %0, i64 %11
  store i32 %10, ptr %12, align 4
  br label %13

13:                                               ; preds = %9
  %14 = add nsw i32 %.01, 1
  br label %7, !llvm.loop !6

15:                                               ; preds = %7
  br label %16

16:                                               ; preds = %23, %15
  %.1 = phi i32 [ %.03, %15 ], [ %22, %23 ]
  %.0 = phi i32 [ 0, %15 ], [ %24, %23 ]
  %17 = icmp slt i32 %.0, %1
  br i1 %17, label %18, label %25

18:                                               ; preds = %16
  %19 = sext i32 %.0 to i64
  %20 = getelementptr inbounds i32, ptr %0, i64 %19
  %21 = load i32, ptr %20, align 4
  %22 = add nsw i32 %.1, %21
  br label %23

23:                                               ; preds = %18
  %24 = add nsw i32 %.0, 1
  br label %16, !llvm.loop !8

25:                                               ; preds = %16
  br label %26

26:                                               ; preds = %25
  %27 = add nsw i32 %.02, 1
  br label %4, !llvm.loop !9

28:                                               ; preds = %4
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
