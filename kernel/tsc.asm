global read_tsc

section .text

read_tsc:
    rdtscp
    shl rdx, 32
    or  rax, rdx
    ret
