; ModuleID = 'test-mem2regSimplify'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_dominanza(ptr noalias noundef %0, ptr noalias noundef %1, i32 noundef %2, i32 noundef %3) #0 {
  br label %5

5:                                                ; preds = %11, %4
  %.01 = phi i32 [ 0, %4 ], [ %12, %11 ]
  %6 = icmp slt i32 %.01, %2
  br i1 %6, label %7, label %13

7:                                                ; preds = %5
  %8 = mul nsw i32 %.01, 2
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds i32, ptr %0, i64 %9
  store i32 %8, ptr %10, align 4
  br label %11

11:                                               ; preds = %7
  %12 = add nsw i32 %.01, 1
  br label %5, !llvm.loop !6

13:                                               ; preds = %5
  %14 = icmp ne i32 %3, 0
  br i1 %14, label %15, label %28

15:                                               ; preds = %13
  br label %16

16:                                               ; preds = %25, %15
  %.0 = phi i32 [ 0, %15 ], [ %26, %25 ]
  %17 = icmp slt i32 %.0, %2
  br i1 %17, label %18, label %27

18:                                               ; preds = %16
  %19 = sext i32 %.0 to i64
  %20 = getelementptr inbounds i32, ptr %0, i64 %19
  %21 = load i32, ptr %20, align 4
  %22 = add nsw i32 %21, 5
  %23 = sext i32 %.0 to i64
  %24 = getelementptr inbounds i32, ptr %1, i64 %23
  store i32 %22, ptr %24, align 4
  br label %25

25:                                               ; preds = %18
  %26 = add nsw i32 %.0, 1
  br label %16, !llvm.loop !8

27:                                               ; preds = %16
  br label %28

28:                                               ; preds = %27, %13
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
!8 = distinct !{!8, !7}
