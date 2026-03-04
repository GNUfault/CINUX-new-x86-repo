global enable_paging
extern pml4_phys_addr

section .text

enable_paging:
    ; Ensure PAE is enabled (CR4.PAE bit = 5) before enabling paging
    mov rax, cr4
    bts rax, 5
    mov cr4, rax

    mov rax, [rel pml4_phys_addr]
    mov cr3, rax

    mov rax, cr0
    bts rax, 31
    mov cr0, rax

    ret
