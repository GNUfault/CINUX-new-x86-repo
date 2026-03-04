global outb
global outw
global outd
global inb
global inw
global ind

section .text

outb:
    mov dx, cx
    mov al, dl
    out dx, al
    ret

outw:
    mov dx, cx
    mov ax, dx
    out dx, ax
    ret

outd:
    mov dx, cx
    mov eax, edx
    out dx, eax
    ret

inb:
    mov dx, cx
    in al, dx
    ret

inw:
    mov dx, cx
    in ax, dx
    ret

ind:
    mov dx, cx
    in eax, dx
    ret
