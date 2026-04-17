; ModuleID = 'test-add-sub.c'
source_filename = "test-add-sub.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @test_add_sub(i32 noundef %0, i32 noundef %1) #0 {
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
  %16 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %17 = load i32, ptr %3, align 4
  %18 = add nsw i32 %17, 15
  store i32 %18, ptr %5, align 4
  %19 = load i32, ptr %5, align 4
  %20 = sub nsw i32 %19, 15
  store i32 %20, ptr %6, align 4
  %21 = load i32, ptr %3, align 4
  %22 = add nsw i32 20, %21
  store i32 %22, ptr %7, align 4
  %23 = load i32, ptr %7, align 4
  %24 = sub nsw i32 %23, 20
  store i32 %24, ptr %8, align 4
  %25 = load i32, ptr %3, align 4
  %26 = sub nsw i32 %25, 7
  store i32 %26, ptr %9, align 4
  %27 = load i32, ptr %9, align 4
  %28 = add nsw i32 %27, 7
  store i32 %28, ptr %10, align 4
  %29 = load i32, ptr %3, align 4
  %30 = sub nsw i32 %29, 8
  store i32 %30, ptr %11, align 4
  %31 = load i32, ptr %11, align 4
  %32 = add nsw i32 8, %31
  store i32 %32, ptr %12, align 4
  %33 = load i32, ptr %3, align 4
  %34 = add nsw i32 %33, 10
  store i32 %34, ptr %13, align 4
  %35 = load i32, ptr %13, align 4
  %36 = sub nsw i32 %35, 5
  store i32 %36, ptr %14, align 4
  %37 = load i32, ptr %3, align 4
  %38 = load i32, ptr %4, align 4
  %39 = add nsw i32 %37, %38
  store i32 %39, ptr %15, align 4
  %40 = load i32, ptr %15, align 4
  %41 = load i32, ptr %4, align 4
  %42 = sub nsw i32 %40, %41
  store i32 %42, ptr %16, align 4
  %43 = load i32, ptr %6, align 4
  %44 = load i32, ptr %8, align 4
  %45 = add nsw i32 %43, %44
  %46 = load i32, ptr %10, align 4
  %47 = add nsw i32 %45, %46
  %48 = load i32, ptr %12, align 4
  %49 = add nsw i32 %47, %48
  %50 = load i32, ptr %14, align 4
  %51 = add nsw i32 %49, %50
  %52 = load i32, ptr %16, align 4
  %53 = add nsw i32 %51, %52
  ret i32 %53
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
