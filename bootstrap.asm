section .multiboot2
align 8
headerStart:
    dd 0xe85250d6
    dd 0
    dd headerStart - headerEnd
    dd 0x100000000 - (0xe85250d6 + 0 + (headerStart - headerEnd))
mmapTag:
    dd 6 ; memory map
    dd .end - mmapTag
    dd 8
    dd 0

.end:

tagEnd:
    ; no tag
    dw 0
    dw 0
    dd 8

headerEnd:

section .text
extern main
global _start

_start:
    mov dword [MBI_ADDR], ebx

    ; setup stack
    mov esp, stackTop
    mov ebp, stackTop

    call setUpGDT

    call main

    cmp eax, 0
    jne .err

    hlt
    jmp $
.err:
    mov edi, test
    call print

    hlt
    jmp $

; edi as str
print:
    mov ebx, [.buffer]
    ; set red color
    mov ah, 0x4
    .loop:
        mov al, [edi]
        cmp al, 0
        je .end
        
        mov word [ebx], ax
        inc edi
        add ebx, 2
        jmp .loop
    .end:
        mov dword [.buffer], ebx 
        ret
    .buffer:
    dd 0xb8000

setUpGDT:
    ;cli
    lgdt [GDTR32]
    jmp (GDT32.CODE - GDT32):.reload
.reload:
    mov ax, GDT32.DATA - GDT32
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ;sti
    ret
    

section .data
global MBI_ADDR
global KERNEL_CS
MBI_ADDR:
    dd 0
KERNEL_CS:
    dw GDT32.CODE - GDT32

section .rodata
test: db "kernel exited with error, halt now", 0

align 8
GDTR32:
    dw GDT32.end - GDT32 - 1
    dd GDT32
align 8
GDT32:
    .NULL:
        dq 0
    .CODE:
        dw 0xffff
        dw 0
        db 0
        db 0x9a
        db 0xcf
        db 0
    .DATA:
        dw 0xffff
        dw 0
        db 0
        db 0x92
        db 0xcf
        db 0
    .end:

section .bss
stackTop:
    resb 4096*4
stackBot: