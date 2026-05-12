; ModuleID = 'LICM-test1-raw.ll'
source_filename = "LICM-test1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @test1_licm(i32 noundef %0, i32 noundef %1) #0 {
  br label %3

3:                                                ; preds = %11, %2
  %.01 = phi i32 [ 0, %2 ], [ %10, %11 ]
  %.0 = phi i32 [ 0, %2 ], [ %12, %11 ]
  %4 = icmp slt i32 %.0, 100
  br i1 %4, label %5, label %13

5:                                                ; preds = %3
  %6 = add nsw i32 %0, %1
  %7 = add nsw i32 %6, 2
  %8 = add nsw i32 %.01, %7
  %9 = add nsw i32 %8, 3
  %10 = add nsw i32 %9, %.0
  br label %11

11:                                               ; preds = %5
  %12 = add nsw i32 %.0, 1
  br label %3, !llvm.loop !6

13:                                               ; preds = %3
  ret i32 %.01
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
