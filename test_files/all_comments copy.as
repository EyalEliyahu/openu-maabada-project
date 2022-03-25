; file ps.as
                        ;.entry LIST
                ;.extern W
;LOOP:   prn #48
;MAIN:   add r3, LIS            T
                        ;macro m1     
;            inc r6
;            mov r3, W
;        endm
;             lea END, r6
;        m1                      
;        sub r1, r4
;        bne END
;        cmp val1, #-6
;        bne END[r15]
;        dec K









;.entry MAIN
;        sub LOOP[r10] ,r14
;END:    stop
;STR:    .string "abcd"
;LIST:   .data 6, -9
;        .data -100
;.entry K
;K:      .data 31
;.extern val1 