#include <SEC_CORE.h>
extern POST4, init_PCIE_config_space
org 0FFFFF941h
use32
POST3:
    mov al, 3
    out POST_PORT, al
    mov esi, POST4
    movd mm7, esi
    jmp POST3_Jmp

;section
org 0FFFFF9F3h

POST3_Jmp:
    mov ebp, Init_HPET_MMIO_and_Compare_Register
    jump init_PCIE_and_host_bridge_config_space