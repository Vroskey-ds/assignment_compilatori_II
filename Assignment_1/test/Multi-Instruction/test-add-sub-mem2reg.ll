; ModuleID = 'test-add-sub.ll'
source_filename = "test-add-sub.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @test_add_sub(i32 noundef %0, i32 noundef %1) #0 {

  %3 = add nsw i32 %0, 15
  %4 = sub nsw i32 %3, 15

  %5 = add nsw i32 20, %0
  %6 = sub nsw i32 %5, 20

  %7 = sub nsw i32 %0, 7
  %8 = add nsw i32 %7, 7

  %9 = sub nsw i32 %0, 8
  %10 = add nsw i32 8, %9

  %11 = add nsw i32 %0, 10
  %12 = sub nsw i32 %11, 5

  %13 = add nsw i32 %0, %1
  %14 = sub nsw i32 %13, %1
  
  %15 = add nsw i32 %4, %6
  %16 = add nsw i32 %15, %8
  %17 = add nsw i32 %16, %10
  %18 = add nsw i32 %17, %12
  %19 = add nsw i32 %18, %14
  ret i32 %19
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
