#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tests/util.h"
#include "src/ff.h"
#include "src/diskio.h"

// Test with 12/16/32.
#define FAT_TYPE (FS_FAT12)

#if FAT_TYPE == FS_FAT12
#define NSEC (1024)
#define NSEC_RAM (NSEC)
#define FAT_FORMAT FM_FAT
#elif FAT_TYPE == FS_FAT16
#define NSEC (10240)
#define NSEC_RAM (1000)
#define FAT_FORMAT FM_FAT
#else
#define NSEC (512000)
#define NSEC_RAM (10000)
#define FAT_FORMAT FM_FAT32
#endif
#define SEC_SIZE (512)

static uint8_t ram_bdev[SEC_SIZE * NSEC_RAM];

struct _bdev_t {
    int pdrv;
};

DRESULT disk_read(void *bdev_in, BYTE *buff, DWORD sector, UINT count) {
    struct _bdev_t *bdev = bdev_in;
    if (sector * SEC_SIZE > sizeof(ram_bdev)) {
        printf("ram_bdev too small\n");
        return FR_DISK_ERR;
    }
    // printf("disk_read(%d, %u, %u)\n", bdev->pdrv, sector, count);
    memcpy(buff, ram_bdev + sector * SEC_SIZE, count * SEC_SIZE);
    return RES_OK;
}

DRESULT disk_write(void *bdev_in, const BYTE *buff, DWORD sector, UINT count) {
    struct _bdev_t *bdev = bdev_in;
    if (sector * SEC_SIZE > sizeof(ram_bdev)) {
        printf("ram_bdev too small\n");
        return FR_DISK_ERR;
    }
    // printf("disk_write(%d, %u, %u)\n", bdev->pdrv, sector, count);
    memcpy(ram_bdev + sector * SEC_SIZE, buff, count * SEC_SIZE);
    return RES_OK;
}

DRESULT disk_ioctl(void *bdev_in, BYTE cmd, void *buff) {
    struct _bdev_t *bdev = bdev_in;
    // printf("disk_ioctl(%d, %u)\n", bdev->pdrv, cmd);
    switch (cmd) {
        case CTRL_SYNC: break;
        case GET_SECTOR_COUNT: *((DWORD*)buff) = NSEC; break;
        case GET_SECTOR_SIZE: *((WORD*)buff) = SEC_SIZE; break;
        case GET_BLOCK_SIZE: *((DWORD*)buff) = 1; break;
        case IOCTL_INIT: *((DSTATUS*)buff) = 0; break;
        case IOCTL_STATUS: *((DSTATUS*)buff) = 0; break;
        default: assert(0);
    }
    return RES_OK;
}

DWORD get_fattime(void) {
    return 0;
}

FRESULT create_file(FATFS* fs, const TCHAR* path, UINT size) {
    uint8_t buf[16] = {0};
    for (uint8_t i = 0; i < sizeof(buf); ++i) {
        buf[i] = i;
    }
    FIL fp;
    FRESULT res = f_open(fs, &fp, path, FA_CREATE_ALWAYS | FA_WRITE);
    for (int i = 0; i < size / sizeof(buf); ++i) {
        UINT written = 0;
        f_write(&fp, buf, sizeof(buf), &written);
    }
    f_close(&fp);
}

FRESULT abandon_chain(FATFS* fs, const TCHAR* path) {
    FIL fp;
    FRESULT res = f_open(fs, &fp, path, FA_READ);

    fp.obj.sclust = 0;
    fp.flag |= 0x40; // FA_MODIFIED
    f_sync(&fp);

    f_close(&fp);
}

FRESULT truncate_without_removing_chain(FATFS* fs, const TCHAR* path, FSIZE_t size) {
    FIL fp;
    FRESULT res = f_open(fs, &fp, path, FA_READ);

    fp.obj.objsize = size;
    fp.flag |= 0x40; // FA_MODIFIED
    f_sync(&fp);

    f_close(&fp);
}

FRESULT corrupt_chain(FATFS* fs, const TCHAR* path) {
    FIL fp;
    uint8_t buf[16] = {0};
    FRESULT res = f_open(fs, &fp, path, FA_WRITE | FA_OPEN_APPEND);

    uint8_t* fat_copy = malloc(fs->fsize * SEC_SIZE);
    memcpy(fat_copy, ram_bdev + fs->fatbase * SEC_SIZE, fs->fsize * SEC_SIZE);

    for (int i = 0; i < 10 * fs->csize * SEC_SIZE / sizeof(buf); ++i) {
        UINT written = 0;
        f_write(&fp, buf, sizeof(buf), &written);
    }

    f_close(&fp);

    f_umount(fs);
    memcpy(ram_bdev + fs->fatbase * SEC_SIZE, fat_copy, fs->fsize * SEC_SIZE);
    free(fat_copy);
    f_mount(fs);
}

int main() {
    struct _bdev_t bdev = {0};
    FATFS fs;
    fs.drv = &bdev;

    uint8_t buf[SEC_SIZE];
    FRESULT res = f_mkfs(&fs, FAT_FORMAT | FM_SFD, 0, buf, sizeof(buf));
    res = f_mount(&fs);

    printf("Cluster size: %d sectors (%d)\n", fs.csize, fs.fs_type);

    DWORD nclst;
    res = f_getfree(&fs, &nclst);
    printf("Free clusters: %u\n", nclst);

    FILINFO fi;

    res = f_mkdir(&fs, "/a");
    res = f_mkdir(&fs, "/a/aa");
    res = f_mkdir(&fs, "/b");
    res = f_mkdir(&fs, "/b/ba");
    res = f_mkdir(&fs, "/c");
    res = f_mkdir(&fs, "/c/ca");
    res = f_mkdir(&fs, "/d");
    res = f_mkdir(&fs, "/d/da");
    create_file(&fs, "/a/aa/aaa.txt", 121);
    create_file(&fs, "/a/aa.txt", 812);
    create_file(&fs, "/a/ab.txt", 38712);
    create_file(&fs, "/a/ac.txt", 0);
    create_file(&fs, "/a/data1.txt", 21171);
    create_file(&fs, "/b/ba/bba.txt", 281);
    create_file(&fs, "/b/ba.txt", 4782);
    create_file(&fs, "/b/bb.txt", 728);
    create_file(&fs, "/b/data2.txt", 23917);
    create_file(&fs, "/c/ca/cca.txt", 24);
    create_file(&fs, "/c/ca.txt", 2971);
    create_file(&fs, "/c/cb.txt", 0);
    create_file(&fs, "/c/data3.txt", fs.csize * SEC_SIZE * 10);
    create_file(&fs, "/d/da/dda.txt", 21);
    create_file(&fs, "/d/da.txt", 3102);
    create_file(&fs, "/d/db.txt", 0);
    create_file(&fs, "/d/data4.txt", 26712);

    printf("\nBefore corruption:\n");
    res = f_getfree(&fs, &nclst);
    printf("Free clusters: %u\n", nclst);
    f_stat(&fs, "/a/data1.txt", &fi);
    printf("/a/data1.txt: %u bytes\n", fi.fsize);
    f_stat(&fs, "/b/data2.txt", &fi);
    printf("/b/data2.txt: %u bytes\n", fi.fsize);
    f_stat(&fs, "/c/data3.txt", &fi);
    printf("/c/data3.txt: %u bytes\n", fi.fsize);
    f_stat(&fs, "/d/data4.txt", &fi);
    printf("/d/data4.txt: %u bytes\n", fi.fsize);

    abandon_chain(&fs, "/a/data1.txt");
    truncate_without_removing_chain(&fs, "/b/data2.txt", 0);
    truncate_without_removing_chain(&fs, "/c/data3.txt", fs.csize * SEC_SIZE * 5);
    corrupt_chain(&fs, "/d/data4.txt");

    printf("\nBefore repair:\n");
    res = f_getfree(&fs, &nclst);
    printf("Free clusters: %u\n", nclst);
    f_stat(&fs, "/a/data1.txt", &fi);
    printf("/a/data1.txt: %u bytes\n", fi.fsize);
    f_stat(&fs, "/b/data2.txt", &fi);
    printf("/b/data2.txt: %u bytes\n", fi.fsize);
    f_stat(&fs, "/c/data3.txt", &fi);
    printf("/c/data3.txt: %u bytes\n", fi.fsize);
    f_stat(&fs, "/d/data4.txt", &fi);
    printf("/d/data4.txt: %u bytes\n", fi.fsize);

    printf("\nRepairing...\n");
    uint8_t bitmap[20];
    res = f_repair(&fs, bitmap, sizeof(bitmap));
    if (res != FR_OK) {
        printf("Repair failed: %d\n", res);
        return 1;
    }

    printf("\nAfter repair:\n");
    res = f_getfree(&fs, &nclst);
    printf("Free clusters: %u\n", nclst);
    f_stat(&fs, "/a/data1.txt", &fi);
    printf("/a/data1.txt: %u bytes\n", fi.fsize);
    f_stat(&fs, "/b/data2.txt", &fi);
    printf("/b/data2.txt: %u bytes\n", fi.fsize);
    f_stat(&fs, "/c/data3.txt", &fi);
    printf("/c/data3.txt: %u bytes\n", fi.fsize);
    f_stat(&fs, "/d/data4.txt", &fi);
    printf("/d/data4.txt: %u bytes\n", fi.fsize);

    f_umount(&fs);

    return 0;
}
