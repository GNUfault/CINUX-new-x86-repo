global sti
global cli
global hlt

section .text

sti:
    sti
    ret

cli:
    cli
    ret

hlt:
    hlt
    ret