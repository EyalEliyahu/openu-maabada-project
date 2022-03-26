; first phase errors from different types

; comments / empty lines
; not good
 ; blablabla
; good
; blablabla

                
    

; line length
; not good (81)
123456789123456789123456789123456789123456789123456789123456789123456789123456789
; good (80)
.string "1234567891234567891234567891234567891234567891234567891234567891234567"

; symbols validations:
TEST: .string "test"
; not good
TEST: .string "test"
r2:   add r3, LIST
123456:   add r3, LIST
a1234567891234567891234567891231:
; good
R3:     add         r3,    LIST
TEST1: .string "test"
TEST2: .data 0,5,6,7

; data validations:
; not good
.data abcasd, 1, 2321, 4
.data 1, -2321,, +4
.data ,1, -2321, +4
.data 1, -2321, +4   ,      
; good
.data 1         ,       -2321   , +4

; string validations
; not good
.string 123456
.string "123456","sdaffdsasadf"
.string ""
; good
.string "this is a test string"


; operands validations
; not good
mov #4,
mov #4
mov #4,, #4
lea #5, LIST
stop test

; good
mov #4, r15