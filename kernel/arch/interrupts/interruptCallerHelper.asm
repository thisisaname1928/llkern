bits 32

section .text

extern globalIDT32R
extern callIretd
global loadIDT

callIretd:
    iretd

loadIDT:
    mov eax, globalIDT32R
    lidt [eax]
    ;sti

    ret
