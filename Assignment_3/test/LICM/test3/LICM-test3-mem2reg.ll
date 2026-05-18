; ModuleID = 'LICM-test3-raw.ll'
source_filename = "LICM-test3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @test3_licm(i32 noundef %0, i32 noundef %1, i32 noundef %2) #0 {
  %4 = icmp sle i32 %0, 0
  br i1 %4, label %7, label %5

5:                                                ; preds = %3
  %6 = icmp sle i32 %1, 0
  br i1 %6, label %7, label %8

7:                                                ; preds = %5, %3
  br label %22

8:                                                ; preds = %5
  br label %9

9:                                                ; preds = %19, %8
  %.03 = phi i32 [ 0, %8 ], [ %13, %19 ]
  %.02 = phi i32 [ 0, %8 ], [ %18, %19 ]
  br label %10

10:                                               ; preds = %15, %9
  %.1 = phi i32 [ %.03, %9 ], [ %13, %15 ]
  %.01 = phi i32 [ 0, %9 ], [ %14, %15 ]
  %11 = mul nsw i32 %0, %2
  %12 = add nsw i32 %11, %.01
  %13 = add nsw i32 %.1, %12
  %14 = add nsw i32 %.01, 1
  br label %15

15:                                               ; preds = %10
  %16 = icmp slt i32 %14, %1
  br i1 %16, label %10, label %17, !llvm.loop !6

17:                                               ; preds = %15
  %18 = add nsw i32 %.02, 1
  br label %19

19:                                               ; preds = %17
  %20 = icmp slt i32 %18, %0
  br i1 %20, label %9, label %21, !llvm.loop !8

21:                                               ; preds = %19
  br label %22

22:                                               ; preds = %21, %7
  %.0 = phi i32 [ 0, %7 ], [ %13, %21 ]
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
