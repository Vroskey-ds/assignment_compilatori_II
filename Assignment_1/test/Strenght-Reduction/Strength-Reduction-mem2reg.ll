; ModuleID = 'Strength-Reduction-raw.ll'
source_filename = "Strength-Reduction.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @strength_reduction(i32 noundef %0, i32 noundef %1) #0 {

  %3 = mul nsw i32 %0, 16
  %4 = mul nsw i32 32, %0

  %5 = mul nsw i32 %0, 15
  %6 = mul nsw i32 31, %0

  %7 = mul nsw i32 %0, 17
  %8 = mul nsw i32 33, %0

  %9 = sdiv i32 %0, 8
  
  %10 = mul nsw i32 %0, 27
  %11 = sdiv i32 %0, 7
  %12 = sdiv i32 16, %0
  %13 = mul nsw i32 %0, %1

  %14 = add nsw i32 %3, %4
  %15 = add nsw i32 %14, %5
  %16 = add nsw i32 %15, %6
  %17 = add nsw i32 %16, %7
  %18 = add nsw i32 %17, %8
  %19 = add nsw i32 %18, %9
  %20 = add nsw i32 %19, %10
  %21 = add nsw i32 %20, %11
  %22 = add nsw i32 %21, %12
  %23 = add nsw i32 %22, %13
  ret i32 %23
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
