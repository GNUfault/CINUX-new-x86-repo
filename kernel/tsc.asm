global read_tsc

section .text

read_tsc:
    rdtsc
    shl rdx, 32
    or  rax, rdx
    ret