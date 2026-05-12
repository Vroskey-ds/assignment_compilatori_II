; ModuleID = 'LICM-test2-mem2reg.ll'
source_filename = "LICM-test2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @test2_licm(i32 noundef %0, i32 noundef %1) #0 {
  %3 = icmp sle i32 %0, 0
  br i1 %3, label %6, label %4

4:                                                ; preds = %2
  %5 = icmp sle i32 %1, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %4, %2
  br label %26

7:                                                ; preds = %4
  %8 = mul nsw i32 %0, 2
  br label %9

9:                                                ; preds = %23, %7
  %.03 = phi i32 [ 0, %7 ], [ %14, %23 ]
  %.02 = phi i32 [ 0, %7 ], [ %22, %23 ]
  %10 = add nsw i32 %.02, %8
  %11 = mul nsw i32 %10, 3
  br label %12

12:                                               ; preds = %19, %9
  %.1 = phi i32 [ %.03, %9 ], [ %14, %19 ]
  %.01 = phi i32 [ 0, %9 ], [ %18, %19 ]
  %13 = add nsw i32 %11, %.01
  %14 = add nsw i32 %.1, %13
  %15 = icmp sgt i32 %14, 1000
  br i1 %15, label %16, label %17

16:                                               ; preds = %12
  br label %21

17:                                               ; preds = %12
  %18 = add nsw i32 %.01, 1
  br label %19

19:                                               ; preds = %17
  %20 = icmp slt i32 %18, %1
  br i1 %20, label %12, label %.loopexit, !llvm.loop !6

.loopexit:                                        ; preds = %19
  br label %21

21:                                               ; preds = %.loopexit, %16
  %22 = add nsw i32 %.02, 1
  br label %23

23:                                               ; preds = %21
  %24 = icmp slt i32 %22, %0
  br i1 %24, label %9, label %25, !llvm.loop !8

25:                                               ; preds = %23
  br label %26

26:                                               ; preds = %25, %6
  %.0 = phi i32 [ 0, %6 ], [ %14, %25 ]
  ret i32 %.0
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
