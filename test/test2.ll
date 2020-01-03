define i64 @foo(i8, i8) {
  %x = zext i8 %0 to i64
  %y = zext i8 %1 to i64
  %z = xor i64 %x, %y
  ret i64 %z
}
