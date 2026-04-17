; ModuleID = 'test-mul-div.c'
source_filename = "test-mul-div.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @test_mul_div(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  %15 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %16 = load i32, ptr %3, align 4
  %17 = mul nsw i32 %16, 42
  store i32 %17, ptr %5, align 4
  %18 = load i32, ptr %5, align 4
  %19 = sdiv i32 %18, 42
  store i32 %19, ptr %6, align 4
  %20 = load i32, ptr %3, align 4
  %21 = mul nsw i32 100, %20
  store i32 %21, ptr %7, align 4
  %22 = load i32, ptr %7, align 4
  %23 = sdiv i32 %22, 100
  store i32 %23, ptr %8, align 4
  %24 = load i32, ptr %3, align 4
  %25 = sdiv i32 %24, 5
  store i32 %25, ptr %9, align 4
  %26 = load i32, ptr %9, align 4
  %27 = mul nsw i32 %26, 5
  store i32 %27, ptr %10, align 4
  %28 = load i32, ptr %9, align 4
  %29 = mul nsw i32 5, %28
  store i32 %29, ptr %11, align 4
  %30 = load i32, ptr %3, align 4
  %31 = mul nsw i32 %30, 10
  store i32 %31, ptr %12, align 4
  %32 = load i32, ptr %12, align 4
  %33 = sdiv i32 %32, 2
  store i32 %33, ptr %13, align 4
  %34 = load i32, ptr %3, align 4
  %35 = load i32, ptr %4, align 4
  %36 = mul nsw i32 %34, %35
  store i32 %36, ptr %14, align 4
  %37 = load i32, ptr %14, align 4
  %38 = load i32, ptr %4, align 4
  %39 = sdiv i32 %37, %38
  store i32 %39, ptr %15, align 4
  %40 = load i32, ptr %6, align 4
  %41 = load i32, ptr %8, align 4
  %42 = add nsw i32 %40, %41
  %43 = load i32, ptr %10, align 4
  %44 = add nsw i32 %42, %43
  %45 = load i32, ptr %11, align 4
  %46 = add nsw i32 %44, %45
  %47 = load i32, ptr %13, align 4
  %48 = add nsw i32 %46, %47
  %49 = load i32, ptr %15, align 4
  %50 = add nsw i32 %48, %49
  ret i32 %50
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
