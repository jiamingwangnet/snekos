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

.PHONY: build
build: $(C_OBJ_FILES) $(ASM_OBJ_FILES)
	mkdir -p dist
	ld --nmagic --output=build/kernel.bin --script=linker.ld $(C_OBJ_FILES) $(ASM_OBJ_FILES) && \
	cp build/kernel.bin bootloader/iso/boot && \
	grub-mkrescue -o dist/SnekOS.iso bootloader/iso