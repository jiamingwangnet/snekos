ASM_SOURCE_FILES := $(shell find src/boot/ -name *.asm)
ASM_OBJ_FILES := $(patsubst src/boot/%.asm, build/boot/%.o, $(ASM_SOURCE_FILES))

C_SOURCE_FILES := $(shell find src/kernel/ -name *.c)
C_OBJ_FILES := $(patsubst src/kernel/%.c, build/kernel/%.o, $(C_SOURCE_FILES))

$(C_OBJ_FILES): build/kernel/%.o : src/kernel/%.c
	mkdir -p $(dir $@) && \
	gcc -mcmodel=large -mno-sse -mno-red-zone -fno-pic -masm=intel -m64 -ffreestanding -c $(patsubst build/kernel/%.o, src/kernel/%.c, $@) -o $@

$(ASM_OBJ_FILES): build/boot/%.o : src/boot/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/boot/%.o, src/boot/%.asm, $@) -o $@

# temporary console font loading
build/assets/vga_font.o: assets/vga_font.psf
	mkdir -p build/assets && \
	objcopy -O elf64-x86-64 -B i386 -I binary assets/vga_font.psf build/assets/vga_font.o

.PHONY: build
build: $(C_OBJ_FILES) $(ASM_OBJ_FILES) build/assets/vga_font.o
	mkdir -p dist
	ld --nmagic --output=build/kernel.bin --script=linker.ld $(C_OBJ_FILES) $(ASM_OBJ_FILES) build/assets/vga_font.o && \
	cp build/kernel.bin bootloader/iso/boot && \
	grub-mkrescue -o dist/SnekOS.iso bootloader/iso