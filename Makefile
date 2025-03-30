NASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld

CFLAGS = -O2 -c -ffreestanding -Ikernel/inc/
LDFLAGS = -n -T linker.ld

ISO_DIR = isofiles/boot
BUILD_DIR = build

XORRISO = xorriso

ASM_SOURCES := $(wildcard boot/*.asm kernel/asm/*.asm kernel/asm/**/*.asm)
C_SOURCES := $(wildcard kernel/src/*.c kernel/src/**/*.c)

ASM_OBJECTS := $(patsubst %, $(BUILD_DIR)/%, $(ASM_SOURCES:%.asm=%.o))
C_OBJECTS := $(patsubst kernel/src/%, $(BUILD_DIR)/kernel/src/%, $(C_SOURCES:%.c=%.o))
OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

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

run: all
	qemu-system-x86_64 -boot d -cdrom os.iso