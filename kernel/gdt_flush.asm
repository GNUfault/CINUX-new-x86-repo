global gdt_flush
global tss_flush

section .text

gdt_flush:
    lgdt [rcx]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    pop rcx
    mov rax, 0x08
    push rax
    push rcx
    retfq

tss_flush:
    mov ax, 0x28
    ltr ax
    ret