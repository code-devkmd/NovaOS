CC = gcc
AS = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -Iinclude
ASFLAGS = -m32
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o kernel.o terminal.o io.o keyboard.o timer.o memory.o paging.o heap.o ata.o fs.o shell.o string.o commands.o serial.o panic.o idt.o isr.o isr_stubs.o gdt.o gdt_flush.o pic.o irq.o irq_stubs.o

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

idt.o: cpu/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

isr.o: cpu/isr.c
	$(CC) $(CFLAGS) -c $< -o $@

isr_stubs.o: cpu/isr_stubs.s
	$(AS) $(ASFLAGS) -c $< -o $@

gdt.o: cpu/gdt.c
	$(CC) $(CFLAGS) -c $< -o $@

gdt_flush.o: cpu/gdt_flush.s
	$(AS) $(ASFLAGS) -c $< -o $@

pic.o: cpu/pic.c
	$(CC) $(CFLAGS) -c $< -o $@

irq.o: cpu/irq.c 
	$(CC) $(CFLAGS) -c $< -o $@ 
irq_stubs.o: cpu/irq_stubs.s 
	$(AS) $(ASFLAGS) -c $< -o $@

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

iso: kernel.bin
	mkdir -p iso/boot iso/grub
	cp kernel.bin iso/boot/kernel.bin
	printf '%s\n' 'set timeout=3' 'set default=0' '' 'menuentry "NovaOS" {' '    multiboot /boot/kernel.bin' '    boot' '}' > iso/grub/grub.cfg
	grub-mkrescue -o NovaOS.iso iso

run: iso
	qemu-system-i386 -cdrom NovaOS.iso

clean:
	rm -f *.o kernel.bin NovaOS.iso
	rm -f iso/boot/kernel.bin

.PHONY: all iso run clean
serial.o: kernel/serial.c
	$(CC) $(CFLAGS) -c $< -o $@

panic.o: kernel/panic.c
	$(CC) $(CFLAGS) -c $< -o $@


timer.o: kernel/timer.c
	$(CC) $(CFLAGS) -c $< -o $@

memory.o: kernel/memory.c
	$(CC) $(CFLAGS) -c $< -o $@

paging.o: kernel/paging.c
	$(CC) $(CFLAGS) -c $< -o $@

heap.o: kernel/heap.c
	$(CC) $(CFLAGS) -c $< -o $@

ata.o: kernel/ata.c
	$(CC) $(CFLAGS) -c $< -o $@

fs.o: kernel/fs.c
	$(CC) $(CFLAGS) -c $< -o $@
