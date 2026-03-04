global enable_paging
extern pml4_phys_addr

section .text

enable_paging:
    mov rax, [rel pml4_phys_addr]
    mov cr3, rax

    mov rax, cr0
    bts rax, 31
    mov cr0, rax

    ret
