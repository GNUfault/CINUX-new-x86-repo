#include <efi.h>
#include <efilib.h>
#include <stdint.h>
#include <cinux/framebuffer.h>

extern framebuffer_t g_fb;
extern void kmain(framebuffer_t *fb_info);

static EFI_STATUS gop_init(EFI_SYSTEM_TABLE *SystemTable) {
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    EFI_STATUS status;

    status = SystemTable->BootServices->LocateProtocol(&gop_guid, NULL, (void **)&gop);
    if (EFI_ERROR(status)) return status;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = NULL;
    UINTN info_size = 0;
    UINTN best_mode = gop->Mode->Mode;
    UINT32 best_w = 0, best_h = 0;

    for (UINTN i = 0; i < gop->Mode->MaxMode; i++) {
        status = gop->QueryMode(gop, i, &info_size, &info);
        if (EFI_ERROR(status)) continue;
        if (info->PixelFormat != PixelRedGreenBlueReserved8BitPerColor &&
            info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) continue;
        if (info->HorizontalResolution == 1024 &&
            info->VerticalResolution   == 768) {
            best_mode = i;
            best_w = info->HorizontalResolution;
            best_h = info->VerticalResolution;
            break;
        }
        if (info->HorizontalResolution > best_w) {
            best_mode = i;
            best_w = info->HorizontalResolution;
            best_h = info->VerticalResolution;
        }
    }

    status = gop->SetMode(gop, best_mode);
    if (EFI_ERROR(status)) return status;

    g_fb.base   = gop->Mode->FrameBufferBase;
    g_fb.width  = gop->Mode->Info->HorizontalResolution;
    g_fb.height = gop->Mode->Info->VerticalResolution;
    g_fb.pitch  = gop->Mode->Info->PixelsPerScanLine * 4;

    return EFI_SUCCESS;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    UINTN map_key, desc_size, map_size = 0;
    UINT32 desc_ver;
    EFI_MEMORY_DESCRIPTOR *map = NULL;
    EFI_STATUS status;

    gop_init(SystemTable);

    status = SystemTable->BootServices->GetMemoryMap(
        &map_size, map, &map_key, &desc_size, &desc_ver
    );
    map_size += desc_size * 2;
    SystemTable->BootServices->AllocatePool(EfiLoaderData, map_size, (void **)&map);
    SystemTable->BootServices->GetMemoryMap(
        &map_size, map, &map_key, &desc_size, &desc_ver
    );

    SystemTable->BootServices->ExitBootServices(ImageHandle, map_key);

    kmain(&g_fb);

    while (1);
}