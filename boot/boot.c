#include <efi.h>
#include <efilib.h>
#include "uefi_boot.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    UINTN map_key, desc_size;
    UINT32 desc_ver;
    EFI_MEMORY_DESCRIPTOR *
    LibMemoryMap(
        UINTN *NoEntries,
        UINTN *MapKey,
        UINTN *DescriptorSize,
        UINT32 *DescriptorVersion
    );
    SystemTable->BootServices->ExitBootServices(ImageHandle, map_key);

    kmain();

    while (1);
}