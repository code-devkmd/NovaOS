CC = gcc
AS = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib
ASFLAGS = -m32
LDFLAGS = -m elf_i386 -T linker.ld

all: iso

boot.o: boot.s
	$(AS) $(ASFLAGS) -c boot.s -o boot.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.bin: boot.o kernel.o terminal.o
	$(LD) $(LDFLAGS) -o kernel.bin boot.o kernel.o terminal.o

terminal.o: terminal.c
	$(CC) $(CFLAGS) -c terminal.c -o terminal.o

iso: kernel.bin
	mkdir -p iso/boot
	cp kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o NovaOS.iso iso

clean:
	rm -f *.o kernel.bin NovaOS.iso