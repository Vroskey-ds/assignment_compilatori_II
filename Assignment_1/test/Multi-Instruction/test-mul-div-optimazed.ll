; ModuleID = 'test-mul-div-mem2reg.ll'
source_filename = "test-mul-div.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @test_mul_div(i32 noundef %0, i32 noundef %1) #0 {

  %3 = mul nsw i32 %0, 42
  %4 = sdiv i32 %3, 42 ; %4 viene bypassata 

  %5 = mul nsw i32 100, %0
  %6 = sdiv i32 %5, 100 ; %6 viene bypassata 

  %7 = sdiv i32 %0, 5
  %8 = mul nsw i32 %7, 5 ; Il passo non ottimizza (la divisione %7 non è 'exact', si perderebbe il resto)
  %9 = mul nsw i32 5, %7 ; Il passo non ottimizza (la divisione %7 non è 'exact', si perderebbe il resto)

  %10 = mul nsw i32 %0, 10
  %11 = sdiv i32 %10, 2 ; Il passo non ottimizza (costanti diverse: 10 e 2)

  %12 = mul nsw i32 %0, %1
  %13 = sdiv i32 %12, %1 ; Il passo non ottimizza (uso di variabile al posto della costante)

  %14 = add nsw i32 %0, %0 ; %14 legge direttamente %0 e %0 (e non %4 e %6 come nell'originale)
  %15 = add nsw i32 %14, %8 ; %15 legge dalla trappola %8
  %16 = add nsw i32 %15, %9 ; %16 legge dalla trappola %9
  %17 = add nsw i32 %16, %11 ; %17 legge dalla trappola %11
  %18 = add nsw i32 %17, %13 ; %18 legge dalla trappola %13
  ret i32 %18
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
