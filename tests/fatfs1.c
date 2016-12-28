#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "tests/util.h"
#include "src/ff.h"
#include "src/diskio.h"

#define NSEC (128)
#define SEC_SIZE (512)
static uint8_t ram_bdev[NSEC * SEC_SIZE];

struct _bdev_t {
    int pdrv;
};

DRESULT disk_read(void *bdev_in, BYTE *buff, DWORD sector, UINT count) {
    struct _bdev_t *bdev = bdev_in;
    printf("disk_read(%d, %u, %u)\n", bdev->pdrv, sector, count);
    memcpy(buff, ram_bdev + sector * SEC_SIZE, count * SEC_SIZE);
    return RES_OK;
}

static DWORD sector_no_hash = -1;
DRESULT disk_write(void *bdev_in, const BYTE *buff, DWORD sector, UINT count) {
    struct _bdev_t *bdev = bdev_in;
    printf("disk_write(%d, %u, %u, ", bdev->pdrv, sector, count);
    if (sector == sector_no_hash) {
        // some sectors have non-deterministic data
        printf("<hash not shown>");
    } else {
        printf("0x%08x", hash_djb2(buff, count * SEC_SIZE));
    }
    printf(")\n");
    memcpy(ram_bdev + sector * SEC_SIZE, buff, count * SEC_SIZE);
    return RES_OK;
}

DRESULT disk_ioctl(void *bdev_in, BYTE cmd, void *buff) {
    struct _bdev_t *bdev = bdev_in;
    printf("disk_ioctl(%d, %u)\n", bdev->pdrv, cmd);
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
    printf("get_fattime()\n");
    return 0;
}

int main() {
    struct _bdev_t bdev = {0};
    FATFS fatfs;
    fatfs.drv = &bdev;

    printf("== MKFS ==\n");
    {
        FRESULT res = f_mkfs(&fatfs, 1, 0);
        printf("mkfs res=%d\n", res);
    }

    printf("== MOUNT ==\n");
    {
        FRESULT res = f_mount(&fatfs);
        printf("mount res=%d\n", res);
    }

    printf("== SET LABEL ==\n");
    {
        FRESULT res = f_setlabel(&fatfs, "LABEL");
        printf("setlabel res=%d\n", res);
    }

    printf("== GET LABEL ==\n");
    {
        uint8_t buf[256];
        DWORD vsn;
        FRESULT res = f_getlabel(&fatfs, (TCHAR*)buf, &vsn);
        printf("getlabel res=%d label=%.12s vsn=%u\n", res, buf, vsn);
    }

    printf("== FILE CREATION ==\n");
    {
        FIL fp;
        FRESULT res = f_open(&fatfs, &fp, "/test.txt", FA_WRITE | FA_CREATE_ALWAYS);
        printf("open res=%d\n", res);
        UINT n;
        res = f_write(&fp, "test file\n", 10, &n);
        printf("write res=%d n=%u\n", res, n);
        sector_no_hash = 34;
        res = f_close(&fp);
        sector_no_hash = -1;
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

    printf("== STAT ==\n");
    {
        FILINFO fno;
        #if _USE_LFN
        fno.lfname = NULL;
        fno.lfsize = 0;
        #endif
        FRESULT res = f_stat(&fatfs, "/test.txt", &fno);
        printf("stat res=%d size=%u date=%u time=%u attrib=%u\n", res, fno.fsize, fno.fdate, fno.ftime, fno.fattrib);
    }

    printf("== FILE CREATION, LONG NAME AND LOTS OF DATA ==\n");
    {
        FIL fp;
        FRESULT res = f_open(&fatfs, &fp, "/filename-that-is-long.txt", FA_WRITE | FA_CREATE_ALWAYS);
        printf("open res=%d\n", res);
        UINT n;
        for (int i = 0; i < 10; ++i) {
            res = f_write(&fp, "More data for the test file.  Need to have enough characters to fill more than one 512-byte sector.\n", 100, &n);
            printf("write res=%d n=%u\n", res, n);
        }
        res = f_close(&fp);
        printf("close res=%d\n", res);
    }

    printf("== FILE SEEK AND READ ==\n");
    {
        FIL fp;
        FRESULT res = f_open(&fatfs, &fp, "/filename-that-is-long.txt", FA_READ);
        printf("open res=%d\n", res);
        res = f_lseek(&fp, 800);
        printf("lseek res=%d\n", res);
        printf("tell %d\n", f_tell(&fp));
        uint8_t buf[100];
        UINT n;
        res = f_read(&fp, buf, 100, &n);
        printf("read res=%d n=%u\n", res, n);
        hexdump(buf, n, 16);
        res = f_close(&fp);
        printf("close res=%d\n", res);
    }

    printf("== MKDIR ==\n");
    {
        FRESULT res = f_mkdir(&fatfs, "/dir");
        printf("mkdir res=%d\n", res);
    }

    printf("== DIRECTORY LISTING ==\n");
    {
        DIR dp;
        FRESULT res = f_opendir(&fatfs, &dp, "/");
        printf("opendir res=%d\n", res);
        FILINFO fno;
        #if _USE_LFN
        char lfn[_MAX_LFN + 1];
        fno.lfname = lfn;
        fno.lfsize = sizeof(lfn);
        #endif
        for (;;){
            res = f_readdir(&dp, &fno);
            if (res != FR_OK || fno.fname[0] == 0) {
                break;
            }
            #if _USE_LFN
            // note: lfname is empty string if it fits in 12 chars in fname
            printf("readdir res=%d size=%u name=/%s/ lname=/%s/\n", res, fno.fsize, fno.fname, fno.lfname);
            #else
            printf("readdir res=%d size=%u name=/%s/\n", res, fno.fsize, fno.fname);
            #endif
        }
        res = f_closedir(&dp);
        printf("closedir res=%d\n", res);
    }

    printf("== RENAME FILE ==\n");
    {
        FRESULT res = f_rename(&fatfs, "/test.txt", "/test2.txt");
        printf("unlink res=%d\n", res);
    }

    printf("== UNLINK FILE ==\n");
    {
        FRESULT res = f_unlink(&fatfs, "/test2.txt");
        printf("unlink res=%d\n", res);
    }

    printf("== RENAME DIR ==\n");
    {
        FRESULT res = f_rename(&fatfs, "/dir", "/dir2");
        printf("unlink res=%d\n", res);
    }

    printf("== UNLINK DIR ==\n");
    {
        FRESULT res = f_unlink(&fatfs, "/dir2");
        printf("unlink res=%d\n", res);
    }

    printf("== FREE SPACE ==\n");
    {
        DWORD nclst;
        FRESULT res = f_getfree(&fatfs, &nclst);
        printf("getfree res=%d nclst=%u\n", res, nclst);
    }

    return 0;
}
