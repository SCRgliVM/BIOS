#include "SEC_CORE.h"
extern GDTAddr, FarPOST2jmp
org 0FFFFF8EBh
use16
POST1: 
    ; Init gdt, enable protected mode & flat memory & SIMD

    ; POST_CODE = 1
    mov al, 1
    out POST_PORT, al

    mov esi, GDTAddr
    o32 lgdt [cs:si]    ; Load gdt
 
    mov eax, cr0        ; Enable Protected Mode & Set Monitor x87 FPU
    or  eax, 3
    mov cr0, eax

    mov eax, cr4        ; Enable support SIMD instruction &
    or  eax, 600h       ; Enable support SIMD Float Point Exception
    mov cr4, eax
    
    mov ax, 14h         ; Set flat address space
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esi, FarPOST2jmp
    jmp dword far [cs:si]