#include <SEC_CORE.h>

org 0FFFFFAACh
use32

JUMP_to_mm7:
    movd esi, mm7
    jmp esi