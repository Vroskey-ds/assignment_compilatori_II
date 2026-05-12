; ModuleID = 'NestedLoop-raw.ll'
source_filename = "NestedLoop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @nested_loops(i32 noundef %0, i32 noundef %1) #0 {
  br label %3                                      ; pre-header del for loop

3:                                                ; preds = %19, %2
  %.02 = phi i32 [ 0, %2 ], [ %.1, %19 ]            ; header del for loop
  %.01 = phi i32 [ 0, %2 ], [ %20, %19 ]
  %4 = icmp slt i32 %.01, %0
  br i1 %4, label %5, label %21                   ; i < n, se vero salto al BB %5 senò al BB %21

5:                                                ; preds = %3
  br label %6                                     ; pre-header del loop interno while

; header del while loop
6:                                                ; preds = %16, %5
  %.1 = phi i32 [ %.02, %5 ], [ %.2, %16 ]
  %.0 = phi i32 [ 0, %5 ], [ %17, %16 ]
  %7 = icmp slt i32 %.0, %1
  br i1 %7, label %8, label %18

; Condizione dell'if - corpo del while
8:                                                ; preds = %6
  %9 = srem i32 %.01, 2
  %10 = icmp eq i32 %9, 0
  br i1 %10, label %11, label %14

; ramo if
11:                                               ; preds = %8
  %12 = mul nsw i32 %.01, %.0
  %13 = add nsw i32 %.1, %12
  br label %16

; ramo else
14:                                               ; preds = %8
  %15 = sub nsw i32 %.1, %.0
  br label %16

; %16 latch del while loop
16:                                               ; preds = %14, %11
  %.2 = phi i32 [ %13, %11 ], [ %15, %14 ]
  %17 = add nsw i32 %.0, 1
  br label %6, !llvm.loop !6



18:                                               ; preds = %6
  br label %19

; latch del while
19:                                               ; preds = %18
  %20 = add nsw i32 %.01, 1                        ; i++
  br label %3, !llvm.loop !8                      ; torno all'header del for loop

21:                                               ; preds = %3
  ret i32 %.02
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
