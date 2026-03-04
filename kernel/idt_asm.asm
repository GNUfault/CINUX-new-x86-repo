global isr_default
global isr_pit
global read_tsc
global lidt_load
global isr_db

extern isr_pit_handler

section .text

isr_default:
    iretq

isr_pit:
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    call isr_pit_handler
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
    iretq

lidt_load:
    lidt [rcx]
    ret

isr_db:
    iretq