; ModuleID = 'test-optimazed.ll'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_indipendente(ptr noalias noundef %0, ptr noalias noundef %1, i32 noundef %2) #0 {
  br label %4 -> preheader loopFuso

4:                                                ; preds = %6, %3 -> header loopFuso
  %.01 = phi i32 [ 0, %3 ], [ %13, %6 ]
  %5 = icmp slt i32 %.01, %2
  br i1 %5, label %6, label %14

6:                                                ; preds = %4  -> Body loopFuso
  %7 = mul nsw i32 %.01, 2
  %8 = sext i32 %.01 to i64
  %9 = getelementptr inbounds i32, ptr %0, i64 %8
  store i32 %7, ptr %9, align 4
  %10 = add nsw i32 %.01, 5
  %11 = sext i32 %.01 to i64
  %12 = getelementptr inbounds i32, ptr %1, i64 %11
  store i32 %10, ptr %12, align 4
  %13 = add nsw i32 %.01, 1
  br label %4, !llvm.loop !6

14:                                               ; preds = %4  -> exitBlock loopFuso
  ret void
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
