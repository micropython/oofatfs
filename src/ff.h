/*--------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.04        (C)ChaN, 2007
/---------------------------------------------------------------------------/
/ FatFs module is an experimenal project to implement FAT file system to
/ cheap microcontrollers. This is a free software and is opened for education,
/ research and development under license policy of following trems.
/
/  Copyright (C) 2007, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is no warranty.
/ * You can use, modify and/or redistribute it for personal, non-profit or
/   profit use without any restriction under your responsibility.
/ * Redistributions of source code must retain the above copyright notice.
/
/---------------------------------------------------------------------------*/

#ifndef _FATFS

#define _MCU_ENDIAN     0
/* The _MCU_ENDIAN defines which access method is used to the FAT structure.
/  1: Enable word access.
/  2: Disable word access and use byte-by-byte access instead.
/  When the architectural byte order of the MCU is big-endian and/or address
/  miss-aligned access is prohibited, the _MCU_ENDIAN must be set to 2.
/  If it is not the case, it can be set to 1 for good code efficiency. */

#define _FS_READONLY    0
/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write, f_sync, f_unlink, f_mkdir, f_chmod, f_rename
/  and useless f_getfree. */

#define _FS_MINIMIZE    0
/* The _FS_MINIMIZE option defines minimization level to remove some functions.
/  0: Full function.
/  1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod and f_rename are removed.
/  2: f_opendir and f_readdir are removed in addition to level 1. */

#define _DRIVES     2
/* Number of logical drives to be used */

#define _USE_SJIS
/* When _USE_SJIS is defined, Shift-JIS code transparency is enabled, otherwise
/  only US-ASCII(7bit) code can be accepted as file/directory name. */

/* #define  _USE_MKFS */
/* When _USE_MKFS is defined and _FS_READONLY is set to 0, f_mkfs function
   is enabled. */


#include "integer.h"


/* Result type for fatfs application interface */
typedef unsigned char   FRESULT;


/* File system object structure */
typedef struct _FATFS {
    BYTE    fs_type;        /* FAT type */
    BYTE    sects_clust;    /* Sectors per cluster */
    BYTE    n_fats;         /* Number of FAT copies */
    BYTE    drive;          /* Physical drive number */
    WORD    id;             /* File system mount ID */
    WORD    n_rootdir;      /* Number of root directory entries */
    DWORD   winsect;        /* Current sector appearing in the win[] */
    DWORD   sects_fat;      /* Sectors per fat */
    DWORD   max_clust;      /* Maximum cluster# + 1 */
    DWORD   fatbase;        /* FAT start sector */
    DWORD   dirbase;        /* Root directory start sector (cluster# for FAT32) */
    DWORD   database;       /* Data start sector */
    DWORD   last_clust;     /* Last allocated cluster */
    BYTE    winflag;        /* win[] dirty flag (1:must be written back) */
    BYTE    win[512];       /* Disk access window for Directory/FAT */
} FATFS;


/* Directory object structure */
typedef struct _DIR {
    FATFS*  fs;         /* Pointer to the owner file system object */
    DWORD   sclust;     /* Start cluster */
    DWORD   clust;      /* Current cluster */
    DWORD   sect;       /* Current sector */
    WORD    index;      /* Current index */
    WORD    id;         /* Sum of owner file system mount ID */
} DIR;


/* File object structure */
typedef struct _FIL {
    FATFS*  fs;             /* Pointer to the owner file system object */
    DWORD   fptr;           /* File R/W pointer */
    DWORD   fsize;          /* File size */
    DWORD   org_clust;      /* File start cluster */
    DWORD   curr_clust;     /* Current cluster */
    DWORD   curr_sect;      /* Current sector */
#if _FS_READONLY == 0
    DWORD   dir_sect;       /* Sector containing the directory entry */
    BYTE*   dir_ptr;        /* Ponter to the directory entry in the window */
#endif
    WORD    id;             /* Sum of owner file system mount ID */
    BYTE    flag;           /* File status flags */
    BYTE    sect_clust;     /* Left sectors in cluster */
    BYTE    buffer[512];    /* File R/W buffer */
} FIL;


/* File status structure */
typedef struct _FILINFO {
    DWORD fsize;            /* Size */
    WORD fdate;             /* Date */
    WORD ftime;             /* Time */
    BYTE fattrib;           /* Attribute */
    char fname[8+1+3+1];    /* Name (8.3 format) */
} FILINFO;



/*-----------------------------------------------------*/
/* FatFs module application interface                  */

FRESULT f_mount (BYTE, FATFS*);                     /* Mount/Unmount a logical drive */
FRESULT f_open (FIL*, const char*, BYTE);           /* Open or create a file */
FRESULT f_read (FIL*, void*, WORD, WORD*);          /* Read data from a file */
FRESULT f_write (FIL*, const void*, WORD, WORD*);   /* Write data to a file */
FRESULT f_lseek (FIL*, DWORD);                      /* Move file pointer of a file object */
FRESULT f_close (FIL*);                             /* Close an open file object */
FRESULT f_opendir (DIR*, const char*);              /* Open an existing directory */
FRESULT f_readdir (DIR*, FILINFO*);                 /* Read a directory item */
FRESULT f_stat (const char*, FILINFO*);             /* Get file status */
FRESULT f_getfree (const char*, DWORD*, FATFS**);   /* Get number of free clusters on the drive */
FRESULT f_sync (FIL*);                              /* Flush cached data of a writing file */
FRESULT f_unlink (const char*);                     /* Delete an existing file or directory */
FRESULT f_mkdir (const char*);                      /* Create a new directory */
FRESULT f_chmod (const char*, BYTE, BYTE);          /* Change file/dir attriburte */
FRESULT f_rename (const char*, const char*);        /* Rename/Move a file or directory */
FRESULT f_mkfs (BYTE, BYTE, BYTE);                  /* Create a file system on the drive */


/* User defined function to give a current time to fatfs module */

DWORD get_fattime (void);   /* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
                            /* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */



/* File function return code (FRESULT) */

#define FR_OK                       0
#define FR_NOT_READY                1
#define FR_NO_FILE                  2
#define FR_NO_PATH                  3
#define FR_INVALID_NAME             4
#define FR_INVALID_DRIVE            5
#define FR_DENIED                   6
#define FR_DISK_FULL                7
#define FR_RW_ERROR                 8
#define FR_WRITE_PROTECTED          9
#define FR_NOT_ENABLED              10
#define FR_NO_FILESYSTEM            11
#define FR_INVALID_OBJECT           12
#define FR_MKFS_ABORTED             13


/* File access control and file status flags (FIL.flag) */

#define FA_READ             0x01
#define FA_OPEN_EXISTING    0x00
#if _FS_READONLY == 0
#define FA_WRITE            0x02
#define FA_CREATE_ALWAYS    0x08
#define FA_OPEN_ALWAYS      0x10
#define FA__WRITTEN         0x20
#define FA__DIRTY           0x40
#endif
#define FA__ERROR           0x80


/* FAT type signature (FATFS.fs_type) */

#define FS_FAT12    1
#define FS_FAT16    2
#define FS_FAT32    3


/* File attribute bits for directory entry */

#define AM_RDO  0x01    /* Read only */
#define AM_HID  0x02    /* Hidden */
#define AM_SYS  0x04    /* System */
#define AM_VOL  0x08    /* Volume label */
#define AM_LFN  0x0F    /* LFN entry */
#define AM_DIR  0x10    /* Directory */
#define AM_ARC  0x20    /* Archive */



/* Multi-byte word access macros  */

#if _MCU_ENDIAN == 1    /* Use word access */
#define LD_WORD(ptr)        (WORD)(*(WORD*)(BYTE*)(ptr))
#define LD_DWORD(ptr)       (DWORD)(*(DWORD*)(BYTE*)(ptr))
#define ST_WORD(ptr,val)    *(WORD*)(BYTE*)(ptr)=(WORD)(val)
#define ST_DWORD(ptr,val)   *(DWORD*)(BYTE*)(ptr)=(DWORD)(val)
#else
#if _MCU_ENDIAN == 2    /* Use byte-by-byte access */
#define LD_WORD(ptr)        (WORD)(((WORD)*(BYTE*)((ptr)+1)<<8)|(WORD)*(BYTE*)(ptr))
#define LD_DWORD(ptr)       (DWORD)(((DWORD)*(BYTE*)((ptr)+3)<<24)|((DWORD)*(BYTE*)((ptr)+2)<<16)|((WORD)*(BYTE*)((ptr)+1)<<8)|*(BYTE*)(ptr))
#define ST_WORD(ptr,val)    *(BYTE*)(ptr)=(BYTE)(val); *(BYTE*)((ptr)+1)=(BYTE)((WORD)(val)>>8)
#define ST_DWORD(ptr,val)   *(BYTE*)(ptr)=(BYTE)(val); *(BYTE*)((ptr)+1)=(BYTE)((WORD)(val)>>8); *(BYTE*)((ptr)+2)=(BYTE)((DWORD)(val)>>16); *(BYTE*)((ptr)+3)=(BYTE)((DWORD)(val)>>24)
#else
#error Don't forget to set _MCU_ENDIAN properly!
#endif
#endif


#define _FATFS
#endif
