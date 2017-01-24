/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file  R0.04    (C)ChaN, 2007
/-----------------------------------------------------------------------*/

#ifndef _DISKIO

#define _READONLY   0   /* 1: Read-only mode */

#include "integer.h"

typedef unsigned char   DSTATUS;
typedef unsigned char   DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (BYTE);
DSTATUS disk_status (BYTE);
DRESULT disk_read (BYTE, BYTE*, DWORD, BYTE);
#if _READONLY == 0
DRESULT disk_write (BYTE, const BYTE*, DWORD, BYTE);
#endif
DRESULT disk_ioctl (BYTE, BYTE, void*);
void    disk_timerproc (void);


/* Results of Disk Functions (DRESULT) */

#define RES_OK          0       /* Successful */
#define RES_ERROR       1       /* R/W Error */
#define RES_WRPRT       2       /* Write Protected */
#define RES_NOTRDY      3       /* Not Ready */
#define RES_PARERR      4       /* Invalid Parameter */


/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT      0x01    /* Drive not initialized */
#define STA_NODISK      0x02    /* No medium in the drive */
#define STA_PROTECT     0x04    /* Write protected */


/* Command code for disk_ioctrl() */

#define GET_SECTORS     1
#define CTRL_POWER      2
#define CTRL_LOCK       3
#define CTRL_EJECT      4
#define MMC_GET_CSD     10
#define MMC_GET_CID     11
#define MMC_GET_OCR     12
#define ATA_GET_REV     20
#define ATA_GET_MODEL   21
#define ATA_GET_SN      22


#define _DISKIO
#endif
