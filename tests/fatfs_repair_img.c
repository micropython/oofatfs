#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tests/util.h"
#include "src/ff.h"
#include "src/diskio.h"

#define SEC_SIZE (512)

int fd;

struct _bdev_t {
    int pdrv;
};

DRESULT disk_read(void *bdev_in, BYTE *buff, DWORD sector, UINT count) {
    struct _bdev_t *bdev = bdev_in;
    lseek(fd, sector * SEC_SIZE, SEEK_SET);
    read(fd, buff, SEC_SIZE * count);
    return RES_OK;
}

DRESULT disk_write(void *bdev_in, const BYTE *buff, DWORD sector, UINT count) {
    struct _bdev_t *bdev = bdev_in;
    lseek(fd, sector * SEC_SIZE, SEEK_SET);
    write(fd, buff, SEC_SIZE * count);
    return RES_OK;
}

DRESULT disk_ioctl(void *bdev_in, BYTE cmd, void *buff) {
    struct _bdev_t *bdev = bdev_in;
    switch (cmd) {
        case CTRL_SYNC:
            fsync(fd);
            break;
        case GET_SECTOR_COUNT:
            *((DWORD*)buff) = lseek(fd, 0, SEEK_END);
            break;
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

int main(int argc, char** argv) {
    struct _bdev_t bdev = {0};
    FATFS fs;
    fs.drv = &bdev;

    fd = open(argv[1], O_RDWR);

    FRESULT res = f_mount(&fs);
    if (res != FR_OK) {
        printf("Failed to mount fs: %d.\n", res);
        return 1;
    }

    printf("Cluster size: %d sectors (%d)\n", fs.csize, fs.fs_type);

    printf("\nBefore repair:\n");
    DWORD nclst;
    res = f_getfree(&fs, &nclst);
    printf("Free clusters: %u\n", nclst);

    printf("\nRepairing...\n");
    // Experiment with the size of the bitmap to see how many reclaimable clusters
    // are left behind for fsck.vfat to find.
    uint8_t bitmap[20];
    res = f_repair(&fs, bitmap, sizeof(bitmap));
    if (res != FR_OK) {
        printf("Repair failed: %d\n", res);
        return 1;
    }

    printf("\nAfter repair:\n");
    res = f_getfree(&fs, &nclst);
    printf("Free clusters: %u\n", nclst);

    f_umount(&fs);

    close(fd);

    return 0;
}
