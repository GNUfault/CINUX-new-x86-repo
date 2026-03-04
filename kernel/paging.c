#include <stdint.h>
#include <stddef.h>
#include <cinux/paging_asm.h>

#define PAGE_SIZE 4096
#define PTE_PRESENT (1ULL << 0)
#define PTE_WRITE (1ULL << 1)
#define PTE_HUGE (1ULL << 7)

__attribute__((aligned(PAGE_SIZE))) uint64_t PML4[512];
__attribute__((aligned(PAGE_SIZE))) uint64_t PDPT[512];
__attribute__((aligned(PAGE_SIZE))) uint64_t PD[512];

uint64_t pml4_phys_addr = 0;

void paging_init(void) {
    for (size_t i = 0; i < 512; i++) {
        PML4[i] = 0;
        PDPT[i] = 0;
        PD[i] = 0;
    }

    uint64_t base = (uint64_t)PML4;
    uint64_t pdpt_phys = (uint64_t)PDPT;
    uint64_t pd_phys   = (uint64_t)PD;

    PML4[0] = pdpt_phys | PTE_PRESENT | PTE_WRITE;
    PDPT[0] = pd_phys | PTE_PRESENT | PTE_WRITE;
    PD[0] = 0x00000000ULL | PTE_PRESENT | PTE_WRITE | PTE_HUGE;

    pml4_phys_addr = base;

    enable_paging();
}
