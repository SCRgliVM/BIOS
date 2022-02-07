#include <SEC_CORE.h>
extern POST3, JUMP_to_mm7
org 0FFFFF930h
use32
POST2: ; Just stub
    mov al, 2
    out POST_PORT, al
    mov esi, POST3
    movd mm7, esi
    jmp JUMP_to_mm7