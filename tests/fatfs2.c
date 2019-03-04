// Test behaviour with different sector sizes

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tests/util.h"
#include "src/ff.h"
#include "src/diskio.h"

struct _bdev_t {
    size_t nsec, sec_size;
    uint8_t *blocks;
};

DRESULT disk_read(void *bdev_in, BYTE *buff, DWORD sector, UINT count) {
    struct _bdev_t *bdev = bdev_in;
    printf("disk_read(%u, %u)\n", sector, count);
    memcpy(buff, bdev->blocks + sector * bdev->sec_size, count * bdev->sec_size);
    return RES_OK;
}

DRESULT disk_write(void *bdev_in, const BYTE *buff, DWORD sector, UINT count) {
    struct _bdev_t *bdev = bdev_in;
    printf("disk_write(%u, %u, 0x%08x)\n", sector, count, hash_djb2(buff, count * bdev->sec_size));
    memcpy(bdev->blocks + sector * bdev->sec_size, buff, count * bdev->sec_size);
    return RES_OK;
}

DRESULT disk_ioctl(void *bdev_in, BYTE cmd, void *buff) {
    struct _bdev_t *bdev = bdev_in;
    printf("disk_ioctl(%u)\n", cmd);
    switch (cmd) {
        case CTRL_SYNC: break;
        case GET_SECTOR_COUNT: *((DWORD*)buff) = bdev->nsec; break;
        case GET_SECTOR_SIZE: *((WORD*)buff) = bdev->sec_size; break;
        case GET_BLOCK_SIZE: *((DWORD*)buff) = 1; break;
        case IOCTL_INIT: *((DSTATUS*)buff) = 0; break;
        case IOCTL_STATUS: *((DSTATUS*)buff) = 0; break;
        default: assert(0);
    }
    return RES_OK;
}

DWORD get_fattime(void) {
    printf("get_fattime()\n");
    return 0;
}

void run_single_test(size_t nsec, size_t sec_size) {
    printf("======== BDEV nsec=%u sec_size=%u ========\n", nsec, sec_size);

    struct _bdev_t bdev = {nsec, sec_size, malloc(nsec * sec_size)};
    if (bdev.blocks == NULL) {
        printf("malloc failed\n");
        return;
    }

    FATFS fatfs;
    fatfs.drv = &bdev;

    printf("== MKFS ==\n");
    {
        uint8_t buf[FF_MAX_SS];
        FRESULT res = f_mkfs(&fatfs, FM_FAT | FM_SFD, 0, buf, sizeof(buf));
        printf("mkfs res=%d\n", res);
        if (res != FR_OK) {
            goto finished;
        }
    }

    printf("== MOUNT ==\n");
    {
        FRESULT res = f_mount(&fatfs);
        printf("mount res=%d\n", res);
    }

    printf("== FILE CREATION ==\n");
    {
        FIL fp;
        FRESULT res = f_open(&fatfs, &fp, "/test.txt", FA_WRITE | FA_CREATE_ALWAYS);
        printf("open res=%d\n", res);
        UINT n;
        res = f_write(&fp, "test file\n", 10, &n);
        printf("write res=%d n=%u\n", res, n);
        res = f_close(&fp);
        printf("close res=%d\n", res);
    }

    printf("== FILE READ ==\n");
    {
        FIL fp;
        FRESULT res = f_open(&fatfs, &fp, "/test.txt", FA_READ);
        printf("open res=%d\n", res);
        uint8_t buf[100];
        UINT n;
        res = f_read(&fp, buf, 100, &n);
        printf("read res=%d n=%u\n", res, n);
        hexdump(buf, n, 16);
        res = f_close(&fp);
        printf("close res=%d\n", res);
    }

finished:
    free(bdev.blocks);
}

int main() {
    // Run tests at the limit of minimum number of sectors for a valid filesystem
    run_single_test(49, 512);
    run_single_test(50, 512);
    run_single_test(33, 1024);
    run_single_test(34, 1024);
    run_single_test(25, 2048);
    run_single_test(26, 2048);
    run_single_test(21, 4096);
    run_single_test(22, 4096);
    return 0;
}
