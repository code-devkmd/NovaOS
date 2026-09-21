#include "fs.h"
#include "ata.h"
#include "heap.h"
#include "terminal.h"
#include "io.h"
#include "string.h"

#define NOVA_MAGIC 0x4E4F5641U
#define INODE_COUNT 64U
#define INODE_SIZE 96U
#define INODE_SECTORS 12U
#define SUPERBLOCK_SECTOR 0U
#define DATA_START (1U + INODE_SECTORS)
#define MAX_BLOCKS 2048U
#define TYPE_FREE 0U
#define TYPE_FILE 1U
#define TYPE_DIR 2U

struct inode_disk
{
    uint32_t used;
    uint32_t type;
    uint32_t parent;
    uint32_t size;
    uint32_t blocks[16];
    char name[16];
};

static struct inode_disk inodes[INODE_COUNT];
static uint32_t cwd = 0;
static int mounted;
static int persistent;
static char pwd_buffer[FS_MAX_PATH + 1];
static unsigned char sector_buffer[512];
static unsigned char ram_data[INODE_COUNT][FS_MAX_FILE_SIZE];

static void zero_mem(void *p, size_t n)
{
    unsigned char *b = (unsigned char *)p;
    while (n--) *b++ = 0;
}

static void copy_bytes(void *dst, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n--) *d++ = *s++;
}

static int starts(const char *s, const char *prefix)
{
    while (*prefix)
    {
        if (*s++ != *prefix++) return 0;
    }
    return 1;
}

static void str_copy(char *dst, const char *src, size_t max)
{
    size_t i = 0;
    if (!max) return;
    while (src[i] && i + 1 < max) { dst[i] = src[i]; i++; }
    dst[i] = 0;
}

static int name_equal(const char *a, const char *b)
{
    return strcmp(a, b) == 0;
}

static int child(uint32_t parent, const char *name)
{
    uint32_t i;
    for (i = 0; i < INODE_COUNT; i++)
        if (inodes[i].used && inodes[i].parent == parent && name_equal(inodes[i].name, name))
            return (int)i;
    return -1;
}

static int next_component(const char **cursor, char *out)
{
    const char *p = *cursor;
    size_t n = 0;
    while (*p == '/') p++;
    if (!*p) { *cursor = p; return 0; }
    while (*p && *p != '/')
    {
        if (n + 1 < 16) out[n++] = *p;
        p++;
    }
    out[n] = 0;
    *cursor = p;
    return 1;
}

static int resolve(const char *path)
{
    uint32_t current = (path[0] == '/') ? 0 : cwd;
    const char *p = path;
    char part[16];
    int found;
    if (!path || !path[0]) return (int)cwd;
    while (next_component(&p, part))
    {
        if (strcmp(part, ".") == 0) continue;
        if (strcmp(part, "..") == 0)
        {
            if (current != 0) current = inodes[current].parent;
            continue;
        }
        found = child(current, part);
        if (found < 0) return -1;
        current = (uint32_t)found;
    }
    return (int)current;
}

static int parent_and_name(const char *path, uint32_t *parent, char *name)
{
    char temp[FS_MAX_PATH + 1];
    size_t len, i;
    int pnode;
    if (!path || !path[0]) return -1;
    str_copy(temp, path, sizeof(temp));
    len = strlen(temp);
    while (len > 1 && temp[len - 1] == '/') temp[--len] = 0;
    i = len;
    while (i > 0 && temp[i - 1] != '/') i--;
    str_copy(name, temp + i, 16);
    if (!name[0] || strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return -1;
    if (i == 0) pnode = (path[0] == '/') ? 0 : (int)cwd;
    else if (i == 1 && temp[0] == '/') pnode = 0;
    else
    {
        temp[i ? i - (temp[i - 1] == '/') : 0] = 0;
        pnode = resolve(temp[0] ? temp : "/");
    }
    if (pnode < 0 || inodes[pnode].type != TYPE_DIR) return -1;
    *parent = (uint32_t)pnode;
    return 0;
}

static int alloc_inode(void)
{
    uint32_t i;
    for (i = 1; i < INODE_COUNT; i++)
        if (!inodes[i].used) return (int)i;
    return -1;
}

static int block_used(uint32_t block)
{
    uint32_t i, j;
    for (i = 0; i < INODE_COUNT; i++) if (inodes[i].used)
        for (j = 0; j < 16; j++) if (inodes[i].blocks[j] == block) return 1;
    return 0;
}

static int alloc_block(void)
{
    uint32_t b;
    for (b = 0; b < MAX_BLOCKS; b++)
        if (!block_used(b)) return (int)b;
    return -1;
}

static int disk_write_inodes(void)
{
    uint32_t s;
    for (s = 0; s < INODE_SECTORS; s++)
    {
        zero_mem(sector_buffer, 512);
        copy_bytes(sector_buffer, ((unsigned char *)inodes) + s * 512, 512);
        if (ata_write_sector(1 + s, sector_buffer) != 0) return -1;
    }
    return 0;
}

static int disk_read_inodes(void)
{
    uint32_t s;
    for (s = 0; s < INODE_SECTORS; s++)
    {
        if (ata_read_sector(1 + s, sector_buffer) != 0) return -1;
        copy_bytes(((unsigned char *)inodes) + s * 512, sector_buffer, 512);
    }
    return 0;
}

static void build_pwd(void)
{
    uint32_t chain[INODE_COUNT];
    uint32_t n = 0, node = cwd, i;
    size_t pos = 0;
    if (cwd == 0) { str_copy(pwd_buffer, "/", sizeof(pwd_buffer)); return; }
    while (node != 0 && n < INODE_COUNT) { chain[n++] = node; node = inodes[node].parent; }
    pwd_buffer[pos++] = '/';
    for (i = n; i > 0; i--)
    {
        size_t j = 0;
        while (inodes[chain[i - 1]].name[j] && pos + 1 < sizeof(pwd_buffer))
            pwd_buffer[pos++] = inodes[chain[i - 1]].name[j++];
        if (i > 1 && pos + 1 < sizeof(pwd_buffer)) pwd_buffer[pos++] = '/';
    }
    pwd_buffer[pos] = 0;
}

int fs_format(void)
{
    uint32_t i;
    uint32_t magic = NOVA_MAGIC;
    uint32_t inode_count = INODE_COUNT;
    uint32_t data_start = DATA_START;
    if (!ata_present()) return -1;
    zero_mem(inodes, sizeof(inodes));
    inodes[0].used = 1;
    inodes[0].type = TYPE_DIR;
    inodes[0].parent = 0;
    str_copy(inodes[0].name, "/", sizeof(inodes[0].name));
    zero_mem(sector_buffer, 512);
    copy_bytes(sector_buffer, &magic, sizeof(magic));
    copy_bytes(sector_buffer + 4, &inode_count, sizeof(inode_count));
    copy_bytes(sector_buffer + 8, &data_start, sizeof(data_start));
    if (ata_write_sector(SUPERBLOCK_SECTOR, sector_buffer) != 0) return -1;
    if (disk_write_inodes() != 0) return -1;
    cwd = 0;
    mounted = 1;
    persistent = 1;
    return 0;
}

int fs_mount(void)
{
    uint32_t magic;
    mounted = 0;
    persistent = 0;
    if (!ata_present())
    {
        zero_mem(inodes, sizeof(inodes));
        inodes[0].used = 1; inodes[0].type = TYPE_DIR; inodes[0].parent = 0; str_copy(inodes[0].name, "/", 16);
        cwd = 0; mounted = 1; return 0;
    }
    if (ata_read_sector(SUPERBLOCK_SECTOR, sector_buffer) != 0) return -1;
    copy_bytes(&magic, sector_buffer, sizeof(magic));
    if (magic != NOVA_MAGIC) return -2;
    if (disk_read_inodes() != 0) return -1;
    if (!inodes[0].used || inodes[0].type != TYPE_DIR) return -1;
    cwd = 0; mounted = 1; persistent = 1;
    return 0;
}

int fs_init(void)
{
    int r = fs_mount();
    if (r == -2)
    {
        if (ata_present())
            return fs_format();

        zero_mem(inodes, sizeof(inodes));
        zero_mem(ram_data, sizeof(ram_data));
        inodes[0].used = 1;
        inodes[0].type = TYPE_DIR;
        inodes[0].parent = 0;
        str_copy(inodes[0].name, "/", 16);
        cwd = 0;
        mounted = 1;
        persistent = 0;
        return 0;
    }
    return r;
}

int fs_sync(void)
{
    if (!mounted) return -1;
    if (!persistent) return 0;
    return disk_write_inodes();
}

int fs_is_mounted(void) { return mounted; }
const char *fs_pwd(void) { build_pwd(); return pwd_buffer; }

int fs_ls(const char *path)
{
    int node = resolve(path && path[0] ? path : ".");
    uint32_t i;
    if (node < 0) return -1;
    if (inodes[node].type == TYPE_FILE)
    {
        terminal_write(inodes[node].name); terminal_putchar('\n'); return 0;
    }
    for (i = 0; i < INODE_COUNT; i++) if (inodes[i].used && inodes[i].parent == (uint32_t)node)
    {
        terminal_write(inodes[i].type == TYPE_DIR ? "d " : "f ");
        terminal_write(inodes[i].name);
        if (inodes[i].type == TYPE_FILE) { terminal_write("  "); print_int((int)inodes[i].size); terminal_write(" B"); }
        terminal_putchar('\n');
    }
    return 0;
}

int fs_cd(const char *path)
{
    int node = resolve(path);
    if (node < 0 || inodes[node].type != TYPE_DIR) return -1;
    cwd = (uint32_t)node;
    return 0;
}

int fs_mkdir(const char *path)
{
    uint32_t parent; char name[16]; int node;
    if (parent_and_name(path, &parent, name) != 0 || child(parent, name) >= 0) return -1;
    node = alloc_inode(); if (node < 0) return -1;
    zero_mem(&inodes[node], sizeof(inodes[node]));
    inodes[node].used = 1; inodes[node].type = TYPE_DIR; inodes[node].parent = parent; str_copy(inodes[node].name, name, 16);
    return fs_sync();
}

int fs_touch(const char *path)
{
    uint32_t parent; char name[16]; int node;
    if (resolve(path) >= 0) return 0;
    if (parent_and_name(path, &parent, name) != 0) return -1;
    node = alloc_inode(); if (node < 0) return -1;
    zero_mem(&inodes[node], sizeof(inodes[node]));
    inodes[node].used = 1; inodes[node].type = TYPE_FILE; inodes[node].parent = parent; str_copy(inodes[node].name, name, 16);
    return fs_sync();
}

int fs_write(const char *path, const char *data)
{
    int node = resolve(path); uint32_t len, blocks, i; const char *p = data;
    if (node < 0 || inodes[node].type != TYPE_FILE) { if (fs_touch(path) != 0) return -1; node = resolve(path); }
    len = (uint32_t)strlen(data); if (len > FS_MAX_FILE_SIZE) len = FS_MAX_FILE_SIZE;
    for (i = 0; i < 16; i++) inodes[node].blocks[i] = 0;
    inodes[node].size = len; blocks = (len + 511U) / 512U;
    for (i = 0; i < blocks; i++)
    {
        int b = alloc_block(); uint32_t chunk = len - i * 512U;
        if (b < 0) return -1; if (chunk > 512) chunk = 512;
        inodes[node].blocks[i] = (uint32_t)b;
        zero_mem(sector_buffer, 512); copy_bytes(sector_buffer, p + i * 512U, chunk);
        if (persistent && ata_write_sector(DATA_START + (uint32_t)b, sector_buffer) != 0) return -1;
        if (!persistent) copy_bytes(ram_data[node] + i * 512U, sector_buffer, 512);
    }
    return fs_sync();
}

int fs_cat(const char *path)
{
    int node = resolve(path); uint32_t i, blocks, remaining;
    if (node < 0 || inodes[node].type != TYPE_FILE) return -1;
    blocks = (inodes[node].size + 511U) / 512U; remaining = inodes[node].size;
    for (i = 0; i < blocks; i++)
    {
        uint32_t n = remaining > 512 ? 512 : remaining, j;
        if (persistent) { if (ata_read_sector(DATA_START + inodes[node].blocks[i], sector_buffer) != 0) return -1; }
        else copy_bytes(sector_buffer, ram_data[node] + i * 512U, 512);
        for (j = 0; j < n; j++) terminal_putchar((char)sector_buffer[j]);
        remaining -= n;
    }
    terminal_putchar('\n'); return 0;
}

int fs_rm(const char *path)
{
    int node = resolve(path); uint32_t i;
    if (node <= 0) return -1;
    if (inodes[node].type == TYPE_DIR)
        for (i = 0; i < INODE_COUNT; i++) if (inodes[i].used && inodes[i].parent == (uint32_t)node) return -2;
    zero_mem(&inodes[node], sizeof(inodes[node]));
    return fs_sync();
}

int fs_cp(const char *src, const char *dst)
{
    int node = resolve(src); uint32_t i, remaining; char data[FS_MAX_FILE_SIZE + 1];
    if (node < 0 || inodes[node].type != TYPE_FILE) return -1;
    if (inodes[node].size > FS_MAX_FILE_SIZE) return -1;
    remaining = inodes[node].size;
    for (i = 0; i < (remaining + 511U) / 512U; i++)
    {
        uint32_t n = remaining > 512 ? 512 : remaining;
        if (persistent) { if (ata_read_sector(DATA_START + inodes[node].blocks[i], sector_buffer) != 0) return -1; }
        else copy_bytes(sector_buffer, ram_data[node] + i * 512U, n);
        copy_bytes(data + i * 512U, sector_buffer, n); remaining -= n;
    }
    data[inodes[node].size] = 0;
    return fs_write(dst, data);
}

int fs_mv(const char *src, const char *dst)
{
    int node = resolve(src); uint32_t parent; char name[16];
    if (node <= 0 || parent_and_name(dst, &parent, name) != 0 || child(parent, name) >= 0) return -1;
    inodes[node].parent = parent; str_copy(inodes[node].name, name, 16); return fs_sync();
}

int fs_stat(const char *path)
{
    int node = resolve(path); if (node < 0) return -1;
    terminal_write("Name: "); terminal_write(inodes[node].name); terminal_putchar('\n');
    terminal_write("Type: "); terminal_write(inodes[node].type == TYPE_DIR ? "directory\n" : "file\n");
    terminal_write("Size: "); print_int((int)inodes[node].size); terminal_write(" bytes\n");
    terminal_write("Inode: "); print_int(node); terminal_write("\n"); return 0;
}

static void find_recursive(uint32_t node, int depth)
{
    uint32_t i; int d;
    for (d = 0; d < depth; d++) terminal_write("  ");
    terminal_write(node == 0 ? "/" : inodes[node].name); terminal_putchar('\n');
    if (inodes[node].type != TYPE_DIR) return;
    for (i = 0; i < INODE_COUNT; i++) if (inodes[i].used && inodes[i].parent == node) find_recursive(i, depth + 1);
}

int fs_find(const char *path)
{
    int node = resolve(path && path[0] ? path : "."); if (node < 0) return -1; find_recursive((uint32_t)node, 0); return 0;
}

int fs_grep(const char *needle, const char *path)
{
    int node = resolve(path); uint32_t i, remaining, len = (uint32_t)strlen(needle); char text[FS_MAX_FILE_SIZE + 1];
    if (node < 0 || inodes[node].type != TYPE_FILE || len == 0) return -1;
    remaining = inodes[node].size;
    for (i = 0; i < (remaining + 511U) / 512U; i++)
    {
        uint32_t n = remaining > 512 ? 512 : remaining;
        if (persistent) { if (ata_read_sector(DATA_START + inodes[node].blocks[i], sector_buffer) != 0) return -1; }
        else copy_bytes(sector_buffer, ram_data[node] + i * 512U, n);
        copy_bytes(text + i * 512U, sector_buffer, n); remaining -= n;
    }
    text[inodes[node].size] = 0;
    if (strstr(text, needle)) { terminal_write(text); terminal_putchar('\n'); return 0; }
    return 1;
}
