ASM=nasm
CC=gcc
CC_FLAGS=-m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-stack-protector -I ./kernel/
LD=ld
LD_FLAGS=-T linker.ld
ASM_SRC=$(shell find . -type f -name "*.asm")
C_SRC=$(shell find . -type f -name "*.c")
OBJ=$(patsubst %.asm, %.$(ASM).o, $(ASM_SRC)) $(patsubst %.c, %.$(CC).o, $(C_SRC))
OUTPUT=test.iso
KERNEL_OUTPUT=llkern

run: $(OUTPUT)
	@qemu-system-x86_64 -cdrom $< -enable-kvm -m 3G -no-reboot -D ./log.txt -monitor stdio

$(OUTPUT): $(KERNEL_OUTPUT)
	@cp $< iso/boot/
	@grub-mkrescue /usr/lib/grub/i386-pc -o $@ iso

$(KERNEL_OUTPUT): $(OBJ)
	@echo "[LD] $< -> $@"
	@$(LD) $(LD_FLAGS) $(OBJ) -o $@

%.nasm.o: %.asm
	@echo "[ASM] $< -> $@"
	@$(ASM) -f elf32 $< -o $@ 

%.gcc.o: %.c
	@echo "[CC] $< -> $@"
	@$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	@rm -f $(KERNEL_OUTPUT) $(OUTPUT) $(OBJ)