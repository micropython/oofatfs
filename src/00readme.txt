FatFs/Tiny-FatFs Module Source Files R0.04   (C)ChaN, 2007


FILES

  ff.h       Common include file for FatFs and application module.
  ff.c       FatFs module.
  tff.h      Common include file for Tiny-FatFs and application module.
  tff.c      Tiny-FatFs module.
  diskio.h   Common include file for (Tiny-)FatFs and disk I/O module.
  integer.h  Alternative type definitions for integer variables.

  Low level disk I/O module is not included in this archive because the
  FatFs/Tiny-FatFs module is only a generic file system layer and not depend
  on any specific storage device. You have to provide a low level disk I/O
  module that written to control your storage device.



CONFIGURATION OPTIONS

  There are several configuration options for various envilonment and
  requirement. The configuration options are defined in header files, ff.h
  and tff.h.

  _MCU_ENDIAN

  This is the most impotant option that depends on the processor architecture.
  When target device corresponds to either or both of following terms, the
  _MCU_ENDIAN must be set to 2 to force FatFs to access FAT structure in
  byte-by-byte. If not the case, this can be set to 1 for good code efficiency.
  The initial value is 0 (must be set to 1 or 2 properly).

  - Muti-byte integers (short, long) are stored in Big-Endian.
  - Address miss-aligned memory access results in an incorrect behavior.


  _FS_READONLY

  When application program does not require any write function, _FS_READONLY
  can be set to 1 to eliminate writing code to reduce module size. This
  setting should be reflected to configurations of low level disk I/O module
  if available. The initial value is 0 (R/W).


  _FS_MINIMIZE

  When application program requires only file read/write function, _FS_MINIMIZE
  can be defined to eliminate some functions to reduce the module size. The
  initial value is 0 (full function).


  _DRIVES

  Number of drives to be used. This option is not supported on Tiny-FatFs.
  The initial value is 2.

  _FAT32

  When _FAT32 is set to 1, the FAT32 support is added with an additional
  code size. This is for only Tiny-FatFs and not supported on FatFs. FatFs
  always supports all FAT type. The initial value is 0 (disabled).


  _USE_SJIS

  When _USE_SJIS is defined, Shift-JIS code set can be used as a file name,
  otherwire second byte of double-byte characters will be collapted. This is
  initially defined.


  _USE_MKFS

  When _USE_MKFS is defined and _FS_READONLY is set to 0, f_mkfs function
  is enabled. This is for only FatFs module and not supported on Tiny-FatFs.
  This is initialy undefined (not available).


  Following table shows which function is removed by configuratin options.

               _FS_MINIMIZE  _FS_MINIMIZE  _FS_READONLY  _USE_MKFS
                    (1)           (2)          (1)        (undef)
   f_mount
   f_open
   f_close
   f_read
   f_lseek
   f_write                                     x
   f_sync                                      x
   f_opendir                      x
   f_readdir                      x
   f_stat            x            x
   f_getfree         x            x            x
   f_unlink          x            x            x
   f_mkdir           x            x            x
   f_chmod           x            x            x
   f_rename          x            x            x
   f_mkfs                                      x            x



AGREEMENTS

  The FatFs/Tiny-FatFs module is a free software and there is no warranty.
  The FatFs/Tiny-FatFs module is opened for education, reserch and development.
  You can use and/or modify it for personal, non-profit or profit use without
  any restriction under your responsibility.



REVISION HISTORY

  Feb 26, 2006  R0.00  Prototype

  Apr 29, 2006  R0.01  First stable version

  Jun 01, 2006  R0.02  Added FAT12. Removed unbuffered mode.
                       Fixed a problem on small (<32M) patition.

  Jun 10, 2006  R0.02a Added a configuration option _FS_MINIMUM.

  Sep 22, 2006  R0.03  Added f_rename().
                       Changed option _FS_MINIMUM to _FS_MINIMIZE.

  Dec 11, 2006  R0.03a Improved cluster scan algolithm to write files fast.
                       Fixed f_mkdir() creates incorrect directory on FAT32.

  Feb 04, 2007  R0.04  Supported multiple drive system.
                       Changed some APIs for multiple drive system.
                       Added f_mkfs().
