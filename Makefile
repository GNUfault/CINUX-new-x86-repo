BUILD   := build
include include.mk

BOOT    := boot

OUT 	:= $(BUILD)/BOOTX64.EFI
IMG     := cinux.img

all: build gnu-efi kernel user link disk

build:
	mkdir -p $(BUILD)

gnu-efi: build
	$(MAKE) -C gnu-efi

boot: gnu-efi
	$(MAKE) -C boot

kernel: boot
	$(MAKE) -C kernel

user: kernel
	$(MAKE) -C user

link: gnu-efi boot kernel user
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

.PHONY: all boot user run clean
