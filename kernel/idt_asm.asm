global lidt_load

section .text

lidt_load:
    lidt [rcx]
    ret