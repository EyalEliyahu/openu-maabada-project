; Here we test that comment doesnt effect output
; and few droplines


; invalid comment (line starts with space)
 ; invalid comment

; valid row - 80 chars
.string "123456789-123456789-123456789-123456789-123456789-123456789-123456789-"

; too long line - 81 chars
.string "123456789-123456789-123456789-123456789-123456789-123456789-123456789-1"


.data 11         ,       -3   , +4
.data ,11,       -3   , +4
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;
mov #1, r15
; the others and invalid
mov #2,
mov #3
mov #4,, #4
;;;;;;;;;;;;;;;;;;;;;;;;;;

A: .string "valid symbol"
A: .string "duplicate symbol error"
r2:   add r3, NON_EXIST_SYMBOL
