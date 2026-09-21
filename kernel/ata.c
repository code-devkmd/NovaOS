#include "ata.h"
#include "io.h"
#include "serial.h"
#include <stdint.h>

#define ATA_DATA 0x1F0
#define ATA_ERROR 0x1F1
#define ATA_SECCOUNT 0x1F2
#define ATA_LBA0 0x1F3
#define ATA_LBA1 0x1F4
#define ATA_LBA2 0x1F5
#define ATA_DRIVE 0x1F6
#define ATA_STATUS 0x1F7
#define ATA_COMMAND 0x1F7
#define ATA_CONTROL 0x3F6
#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30
#define ATA_CMD_IDENTIFY 0xEC
#define ATA_SR_BSY 0x80
#define ATA_SR_DRQ 0x08
#define ATA_SR_ERR 0x01

static int present;

static uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}
static void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}
static uint16_t inw(uint16_t port)
{
    uint16_t value;
    __asm__ volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}
static void io_wait(void) { outb(0x80, 0); }

static int wait_not_busy(void)
{
    unsigned int i;
    for (i = 0; i < 1000000U; i++)
    {
        uint8_t s = inb(ATA_STATUS);
        if (!(s & ATA_SR_BSY)) return (s & ATA_SR_ERR) ? -1 : 0;
    }
    return -1;
}

static int wait_drq(void)
{
    unsigned int i;
    for (i = 0; i < 1000000U; i++)
    {
        uint8_t s = inb(ATA_STATUS);
        if (s & ATA_SR_ERR) return -1;
        if ((s & ATA_SR_DRQ) && !(s & ATA_SR_BSY)) return 0;
    }
    return -1;
}

int ata_init(void)
{
    uint16_t id[256];
    unsigned int i;
    outb(ATA_CONTROL, 0);
    outb(ATA_DRIVE, 0xA0);
    outb(ATA_SECCOUNT, 0);
    outb(ATA_LBA0, 0);
    outb(ATA_LBA1, 0);
    outb(ATA_LBA2, 0);
    outb(ATA_COMMAND, ATA_CMD_IDENTIFY);
    if (inb(ATA_STATUS) == 0)
    {
        present = 0;
        serial_write("[ATA] No primary master disk\n");
        return -1;
    }
    if (wait_drq() != 0)
    {
        present = 0;
        serial_write("[ATA] IDENTIFY failed\n");
        return -1;
    }
    for (i = 0; i < 256; i++) id[i] = inw(ATA_DATA);
    (void)id;
    present = 1;
    serial_write("[ATA] Primary master detected\n");
    return 0;
}

int ata_read_sector(uint32_t lba, void *buffer)
{
    uint16_t *dst = (uint16_t *)buffer;
    unsigned int i;
    if (!present || lba >= 0x10000000U) return -1;
    if (wait_not_busy() != 0) return -1;
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA0, (uint8_t)lba);
    outb(ATA_LBA1, (uint8_t)(lba >> 8));
    outb(ATA_LBA2, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, ATA_CMD_READ);
    if (wait_drq() != 0) return -1;
    for (i = 0; i < 256; i++) dst[i] = inw(ATA_DATA);
    return 0;
}

int ata_write_sector(uint32_t lba, const void *buffer)
{
    const uint16_t *src = (const uint16_t *)buffer;
    unsigned int i;
    if (!present || lba >= 0x10000000U) return -1;
    if (wait_not_busy() != 0) return -1;
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA0, (uint8_t)lba);
    outb(ATA_LBA1, (uint8_t)(lba >> 8));
    outb(ATA_LBA2, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, ATA_CMD_WRITE);
    if (wait_drq() != 0) return -1;
    for (i = 0; i < 256; i++) __asm__ volatile("outw %%ax, %%dx" : : "a"(src[i]), "d"((uint16_t)ATA_DATA));
    io_wait();
    return wait_not_busy();
}

int ata_present(void) { return present; }
