# Include this in (every) Makefile
# NOTE: Make sure to define BUILD before using this

SUDO      = sudo
CC        = clang
AS        = nasm
LD        = lld-link

BITS     := 64
MARCH    := penryn
MTUNE    := penryn
OPTIMIZE := 2
GDB      := 2
TARGET   := x86_64-w64-windows-gnu

CFLAGS   := -m$(BITS) -march=$(MARCH) -mtune=$(MTUNE) -O$(OPTIMIZE) -g$(GDB) -target $(TARGET)

CFLAGS   += -I/usr/include/efi -I/usr/include/efi/x86_64
CFLAGS   += -ffreestanding -fshort-wchar -mno-red-zone
CFLAGS   += -Wall -Wextra

ASFLAGS  := -f bin

LDFLAGS  := /nodefaultlib /subsystem:efi_application /entry:efi_main

%.o: %.c
	$(CC) $(CFLAGS) -c $(shell pwd)/$< -o $(BUILD)/$@
