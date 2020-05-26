; ModuleID = 'main'
source_filename = "main"

@c = global [11 x i32] zeroinitializer
@d = global [11 x i32] zeroinitializer
@i = global i32 0
@aaa = global i32 0
@bbb = global i32 0
@ccc = global i32 0
@ddd = global i32 0
@.str = constant [4 x i8] c"%d\0A\00"
@.str.1 = constant [4 x i8] c"%d\0A\00"
@.str.2 = constant [4 x i8] c"%d\0A\00"

define internal void @main() {
entrypoint:
  store i32 0, i32* @i
  br label %cond

cond:                                             ; preds = %loop, %entrypoint
  %tmp = load i32, i32* @i
  %0 = icmp sle i32 %tmp, 10
  %forCond = icmp ne i1 %0, false
  br i1 %forCond, label %loop, label %afterLoop

loop:                                             ; preds = %cond
  %tmp1 = load i32, i32* @i
  %tmp2 = load i32, i32* @i
  %subtmpi = sub i32 %tmp2, 0
  %1 = getelementptr inbounds [11 x i32], [11 x i32]* @c, i32 0, i32 %subtmpi
  store i32 %tmp1, i32* %1
  %tmp3 = load i32, i32* @i
  %subtmpi4 = sub i32 10, %tmp3
  %tmp5 = load i32, i32* @i
  %subtmpi6 = sub i32 %tmp5, 0
  %2 = getelementptr inbounds [11 x i32], [11 x i32]* @c, i32 0, i32 %subtmpi6
  store i32 %subtmpi4, i32* %2
  %3 = add i32 %tmp, 1
  store i32 %3, i32* @i
  br label %cond

afterLoop:                                        ; preds = %cond
  store i32 1, i32* @aaa
  store i32 0, i32* @i
  br label %cond7

cond7:                                            ; preds = %loop8, %afterLoop
  %tmp10 = load i32, i32* @i
  %4 = icmp sle i32 %tmp10, 10
  %forCond11 = icmp ne i1 %4, false
  br i1 %forCond11, label %loop8, label %afterLoop9

loop8:                                            ; preds = %cond7
  %tmp12 = load i32, i32* @i
  %subtmpi13 = sub i32 %tmp12, 0
  %5 = getelementptr inbounds [11 x i32], [11 x i32]* @c, i32 0, i32 %subtmpi13
  %arrRef = load i32, i32* %5
  %tmp14 = load i32, i32* @i
  %subtmpi15 = sub i32 %tmp14, 0
  %6 = getelementptr inbounds [11 x i32], [11 x i32]* @c, i32 0, i32 %subtmpi15
  %arrRef16 = load i32, i32* %6
  %multmpi = mul i32 %arrRef, %arrRef16
  %tmp17 = load i32, i32* @i
  %subtmpi18 = sub i32 %tmp17, 0
  %7 = getelementptr inbounds [11 x i32], [11 x i32]* @c, i32 0, i32 %subtmpi18
  store i32 %multmpi, i32* %7
  %8 = add i32 %tmp10, 1
  store i32 %8, i32* @i
  br label %cond7

afterLoop9:                                       ; preds = %cond7
  store i32 1, i32* @bbb
  store i32 0, i32* @i
  br label %cond19

cond19:                                           ; preds = %merge, %afterLoop9
  %tmp22 = load i32, i32* @i
  %9 = icmp sle i32 %tmp22, 10
  %forCond23 = icmp ne i1 %9, false
  br i1 %forCond23, label %loop20, label %afterLoop21

loop20:                                           ; preds = %cond19
  %tmp24 = load i32, i32* @i
  %subtmpi25 = sub i32 %tmp24, 0
  %10 = getelementptr inbounds [11 x i32], [11 x i32]* @c, i32 0, i32 %subtmpi25
  %arrRef26 = load i32, i32* %10
  %printf = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %arrRef26)
  %tmp27 = load i32, i32* @i
  %subtmpi28 = sub i32 %tmp27, 0
  %11 = getelementptr inbounds [11 x i32], [11 x i32]* @c, i32 0, i32 %subtmpi28
  %arrRef29 = load i32, i32* %11
  %tmpEQ = icmp eq i32 %arrRef29, 2
  %ifCond = icmp ne i1 %tmpEQ, false
  br i1 %ifCond, label %then, label %else

afterLoop21:                                      ; preds = %cond19
  store i32 1, i32* @ccc
  %printf32 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i32 0, i32 0), i32 11112)
  store i32 0, i32* @i
  br label %cond33

then:                                             ; preds = %loop20
  %tmp30 = load i32, i32* @i
  %subtmpi31 = sub i32 %tmp30, 0
  %12 = getelementptr inbounds [11 x i32], [11 x i32]* @c, i32 0, i32 %subtmpi31
  store i32 1, i32* %12
  br label %merge

else:                                             ; preds = %loop20
  br label %merge

merge:                                            ; preds = %else, %then
  %13 = add i32 %tmp22, 1
  store i32 %13, i32* @i
  br label %cond19

cond33:                                           ; preds = %loop34, %afterLoop21
  %tmp36 = load i32, i32* @i
  %14 = icmp sle i32 %tmp36, 10
  %forCond37 = icmp ne i1 %14, false
  br i1 %forCond37, label %loop34, label %afterLoop35

loop34:                                           ; preds = %cond33
  %tmp38 = load i32, i32* @i
  %subtmpi39 = sub i32 %tmp38, 0
  %15 = getelementptr inbounds [11 x i32], [11 x i32]* @c, i32 0, i32 %subtmpi39
  %arrRef40 = load i32, i32* %15
  %printf41 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.2, i32 0, i32 0), i32 %arrRef40)
  %16 = add i32 %tmp36, 1
  store i32 %16, i32* @i
  br label %cond33

afterLoop35:                                      ; preds = %cond33
  store i32 1, i32* @ddd
  ret void
}

declare i32 @printf(i8*, ...)

declare i32 @scanf(...)
