; ModuleID = 'Strength-Reduction-mem2reg.ll'
source_filename = "Strength-Reduction.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @strength_reduction(i32 noundef %0, i32 noundef %1) #0 {

  %3 = shl i32 %0, 4
  %4 = mul nsw i32 %0, 16 ; %4 viene bypassata dal nuovo shift %3

  %5 = shl i32 %0, 5
  %6 = mul nsw i32 32, %0 ; %6 viene bypassata dal nuovo shift %5

  %7 = shl i32 %0, 4
  %8 = sub i32 %7, %0
  %9 = mul nsw i32 %0, 15 ; %9 viene bypassata dalla nuova sub %8

  %10 = shl i32 %0, 5
  %11 = sub i32 %10, %0
  %12 = mul nsw i32 31, %0 ; %12 viene bypassata dalla nuova sub %11

  %13 = shl i32 %0, 4
  %14 = add i32 %13, %0
  %15 = mul nsw i32 %0, 17 ; %15 viene bypassata dalla nuova add %14

  %16 = shl i32 %0, 5
  %17 = add i32 %16, %0
  %18 = mul nsw i32 33, %0 ; %18 viene bypassata dalla nuova add %17

  %19 = ashr i32 %0, 3
  %20 = sdiv i32 %0, 8 ; %20 viene bypassata dal nuovo ashr %19

  %21 = mul nsw i32 %0, 27 ; Il passo non ottimizza (costante non valida)
  %22 = sdiv i32 %0, 7 ; Il passo non ottimizza (costante non valida)
  %23 = sdiv i32 16, %0 ; Il passo verifica solo il secondo operando
  %24 = mul nsw i32 %0, %1 ; Variabile al posto della costante

  %25 = add nsw i32 %3, %5 ; %25 legge i nuovi shift %3 e %5 (e non %3 e %4 come nell'originale)
  %26 = add nsw i32 %25, %8 ; %26 legge dalla sub %8
  %27 = add nsw i32 %26, %11 ; %27 legge dalla sub %11
  %28 = add nsw i32 %27, %14 ; %28 legge dall'add %14
  %29 = add nsw i32 %28, %17 ; %29 legge dall'add %17
  %30 = add nsw i32 %29, %19 ; %30 legge dall'ashr %19
  
  %31 = add nsw i32 %30, %21
  %32 = add nsw i32 %31, %22
  %33 = add nsw i32 %32, %23
  %34 = add nsw i32 %33, %24
  ret i32 %34
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
