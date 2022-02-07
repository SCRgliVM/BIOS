#include "SEC_CORE.h"
org 0FFFFF8D0h
use16
entry:

    fninit             ; Start init x87 FPU
    movd mm0, eax      ; Save state of Processor Built-In Self-Test(BIST)

    rdtsc              ; Get and save Time Stamp Counter in mm5:mm6
    movd mm5, edx
    movd mm6, eax

    ; Reset Generator I/0 port 
    mov dx, RST_CNT
    in al, dx
    cmp al, 4
    jnz short POST1
    ; IF RST_CNT == 4 { ; Maybe Reset?
    mov dx, RST_CNT
    mov al, 6
    out dx, al
    ; }