NASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld

XORRISO = xorriso

QEMU = qemu-system-x86_64
BOCHS = bochs

CFLAGS = -Wall -O2 -c -ffreestanding -Ikernel/inc/
LDFLAGS = -n -T linker.ld

ISO_DIR = isofiles/boot
BUILD_DIR = build

ASM_SOURCES := $(shell find boot kernel/asm -type f -name "*.asm")
C_SOURCES := $(shell find kernel/src -type f -name "*.c")

ASM_OBJECTS := $(ASM_SOURCES:%.asm=$(BUILD_DIR)/%.o)
C_OBJECTS := $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)

OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

all: kernel.bin createiso

$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)   
	$(NASM) -f elf32 $< -o $@

$(BUILD_DIR)/kernel/src/%.o: kernel/src/%.c
	@mkdir -p $(dir $@)   
	$(CC) $(CFLAGS) $< -o $@

kernel.bin: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD_DIR)
	rm -f isofiles/boot/kernel.bin
	rm -f kernel.bin os.iso
	rm -f bochslog.txt

createiso:
	cp kernel.bin $(ISO_DIR)
	cat isofiles/boot/grub/i386-pc/cdboot.img core.img > isofiles/boot/grub/boot.img
	$(XORRISO) -as mkisofs -R -J -b boot/grub/boot.img -no-emul-boot -boot-load-size 4 -boot-info-table -o os.iso isofiles

qemu:
	$(QEMU) -boot d -cdrom os.iso

bochs:
	$(BOCHS) -f bochsrc.bxrc -q

run: all qemu