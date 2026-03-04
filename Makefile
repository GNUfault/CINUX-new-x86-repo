BUILD   := build
include include.mk

BOOT    := boot

OBJS    := $(shell ls $(BUILD)/*.o)

OUT 	:= $(BUILD)/BOOTX64.EFI

all: build gnu-efi kernel link disk

build:
	mkdir -p $(BUILD)

gnu-efi: build
	$(MAKE) -C gnu-efi

kernel: gnu-efi
	$(MAKE) -C kernel

link: gnu-efi kernel
	lld-link $(LDFLAGS) /out:$(OUT) $(OBJS)

disk: kernel
	dd if=/dev/zero of=cinux.img bs=1k count=1440
	mformat -i cinux.img -f 1440 ::
	mmd -i cinux.img ::/EFI
	mmd -i cinux.img ::/EFI/BOOT
	mcopy -i cinux.img $(OUT) ::/EFI/BOOT

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

clean:
	rm -rf $(BUILD)

.PHONY: all boot disk clean
