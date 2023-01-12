CFLAGS = -mcmodel=large -mno-sse -mno-red-zone -fno-pic -masm=intel -m64 -O3 -ffreestanding -Wall -Wextra
DEFINES = OPTIMISE_GRAPHICS

ASM_SOURCE_FILES := $(shell find src/ -name *.asm)
ASM_OBJ_FILES := $(patsubst src/%.asm, build/%.o, $(ASM_SOURCE_FILES))

C_SOURCE_FILES := $(shell find src/ -name *.c)
C_OBJ_FILES := $(patsubst src/%.c, build/%.o, $(C_SOURCE_FILES))

$(C_OBJ_FILES): build/%.o : src/%.c
	mkdir -p $(dir $@) && \
	gcc $(CFLAGS) -c $(patsubst build/%.o, src/%.c, $@) -o $@ -D ${DEFINES}

$(ASM_OBJ_FILES): build/%.o : src/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/%.o, src/%.asm, $@) -o $@ -D ${DEFINES}

# temporary console font loading
build/assets/vga_font.o: assets/vga_font.psf
	mkdir -p build/assets && \
	objcopy -O elf64-x86-64 -B i386 -I binary assets/vga_font.psf build/assets/vga_font.o

.PHONY: build, clean
build: $(C_OBJ_FILES) $(ASM_OBJ_FILES) build/assets/vga_font.o
	mkdir -p dist
	ld --nmagic --output=build/kernel.bin --script=linker.ld $(C_OBJ_FILES) $(ASM_OBJ_FILES) build/assets/vga_font.o && \
	cp build/kernel.bin bootloader/iso/boot && \
	grub-mkrescue -o dist/SnekOS.iso bootloader/iso

clean: 
	rm -r ./build
	mkdir build
	rm ./bootloader/iso/boot/kernel.bin