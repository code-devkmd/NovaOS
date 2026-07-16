CC = gcc
AS = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib
ASFLAGS = -m32
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o kernel.o terminal.o io.o keyboard.o shell.o string.o commands.o

all: iso

boot.o: boot/boot.s
	$(AS) $(ASFLAGS) -c $< -o $@

kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

terminal.o: kernel/terminal.c
	$(CC) $(CFLAGS) -c $< -o $@

io.o: kernel/io.c
	$(CC) $(CFLAGS) -c $< -o $@

keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

shell.o: kernel/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

string.o: kernel/string.c
	$(CC) $(CFLAGS) -c $< -o $@

commands.o: kernel/commands.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

iso: kernel.bin
	mkdir -p iso/boot
	cp kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o NovaOS.iso iso

run: iso
	qemu-system-i386 -cdrom NovaOS.iso

clean:
	rm -f *.o kernel.bin NovaOS.iso
	rm -f iso/boot/kernel.bin

.PHONY: all iso run clean