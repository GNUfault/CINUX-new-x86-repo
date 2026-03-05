SUDO              = sudo
CC                = clang
AS                = nasm
LD                = lld-link

BITS             := 64
MARCH            := penryn
MTUNE            := penryn
OPTIMIZE         := 2
GDB              := 2
STD              := c99
TARGET           := x86_64-w64-windows-gnu

IMG              := cinux.img

CFLAGS           := -m$(BITS) -march=$(MARCH) -mtune=$(MTUNE) -O$(OPTIMIZE) -g$(GDB) -std=$(STD) -target $(TARGET)

CFLAGS           += -I/usr/include/efi -I/usr/include/efi/x86_64 -Iinclude
CFLAGS           += -ffreestanding -fshort-wchar -mno-red-zone
CFLAGS           += -Wall -Wextra

AS_OUTPUT_FORMAT := win64

ASFLAGS          := -f $(AS_OUTPUT_FORMAT)

LDFLAGS          := /nodefaultlib /subsystem:efi_application /entry:efi_main

BUILD            := build
OUT 	         := $(BUILD)/BOOTX64.EFI

all: setup c asm link disk

setup:
	mkdir -p $(BUILD)

c: setup
	for f in $(shell find . -name '*.c'); do \
		$(CC) $(CFLAGS) -c $$f -o $(BUILD)/$$(basename $${f} .c).o; \
	done

asm: setup
	for f in $(shell find . -name '*.asm'); do \
		$(AS) $(ASFLAGS) $$f -o $(BUILD)/$$(basename $${f} .asm).o; \
	done

link: build
	lld-link $(LDFLAGS) /out:$(OUT) $(shell ls $(BUILD)/*.o)

disk: gnu-efi boot kernel user
	dd if=/dev/zero of=$(IMG) bs=1k count=1440
	mformat -i $(IMG) -f 1440 ::
	mmd -i $(IMG) ::/EFI
	mmd -i $(IMG) ::/EFI/BOOT
	mcopy -i $(IMG) $(OUT) ::/EFI/BOOT

run:
	$(SUDO) \
	qemu-system-x86_64 \
	-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/ovmf/OVMF_CODE.fd \
	-drive if=pflash,format=raw,file=/usr/share/edk2/ovmf/OVMF_VARS.fd \
	-device usb-ehci,id=ehci \
	-drive id=usbdisk,if=none,file=cinux.img,format=raw \
	-device usb-storage,drive=usbdisk \
	-enable-kvm \
	-serial stdio

debug:
	$(SUDO) \
	qemu-system-x86_64 \
	-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/ovmf/OVMF_CODE.fd \
	-drive if=pflash,format=raw,file=/usr/share/edk2/ovmf/OVMF_VARS.fd \
	-device usb-ehci,id=ehci \
	-drive id=usbdisk,if=none,file=cinux.img,format=raw \
	-device usb-storage,drive=usbdisk \
	-enable-kvm \
	-serial stdio \
	-s \
	-S \
	-no-reboot \
	-no-shutdown

clean:
	rm -rf $(BUILD) $(IMG)

