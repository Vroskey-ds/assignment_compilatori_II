; ModuleID = 'Strength-Reduction-optimazed.ll'
source_filename = "Strength-Reduction.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @strength_reduction(i32 noundef %0, i32 noundef %1) #0 {
  %3 = shl i32 %0, 4
  %4 = shl i32 %0, 5
  %5 = shl i32 %0, 4
  %6 = sub i32 %5, %0
  %7 = shl i32 %0, 5
  %8 = sub i32 %7, %0
  %9 = shl i32 %0, 4
  %10 = add i32 %9, %0
  %11 = shl i32 %0, 5
  %12 = add i32 %11, %0
  %13 = ashr i32 %0, 3
  %14 = mul nsw i32 %0, 27
  %15 = sdiv i32 %0, 7
  %16 = sdiv i32 16, %0
  %17 = mul nsw i32 %0, %1
  %18 = add nsw i32 %3, %4
  %19 = add nsw i32 %18, %6
  %20 = add nsw i32 %19, %8
  %21 = add nsw i32 %20, %10
  %22 = add nsw i32 %21, %12
  %23 = add nsw i32 %22, %13
  %24 = add nsw i32 %23, %14
  %25 = add nsw i32 %24, %15
  %26 = add nsw i32 %25, %16
  %27 = add nsw i32 %26, %17
  ret i32 %27
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
