FatFs/Tiny-FatFs Module Source Files R0.02  (C)ChaN, 2006


Files

  ff.h       Common include file for FatFs and application module.
  ff.c       FatFs module.
  tff.h      Common include file for Tiny-FatFs and application module.
  tff.c      Tiny-FatFs module.
  diskio.h   Common include file for (Tiny-)FatFs and disk I/O module.
  integer.h  Alternative type definitions for unsigned integers.

  Low level disk I/O module is not included in this archive because the
  FatFs/Tiny-FatFs module is only a generic file system layer and not depend
  on any specific storage device. You have to provide a low level disk I/O
  module that written to control your storage device.



Configuration Options

  There are several configuration options for various envilonment and requirement.
  The configuration options are in include files, ff.h and tff.h.

  #define _BYTE_ACC

  This is the most impotant option that depends on the processor architecture.
  When your microcontroller corresponds to either or both of following terms, the
  _BYTE_ACC must be defined.

  - Muti-byte integers (short, long) are stored in Big-Endian.
  - Address unaligned word access causes an address error or incorrect behavior.


  #define _FS_READONLY

  When application program does not require any write function, _FS_READONLY can be
  defined to eliminate writing code to reduce module size.


  #define _USE_SJIS

  When _USE_SJIS is defined, Shift-JIS code set can be used as a file name,
  otherwire second byte of double-byte characters will be collapted.



Agreements

  The FatFs/Tiny-FatFs module is a free software and there is no warranty.
  The FatFs/Tiny-FatFs module is opened for education, reserch and development.
  You can use, modify and republish it for personal, non-profit or profit use
  without any limitation under your responsibility.



Revision History

  Feb 26, 2006  R0.00  Prototype
  Apr 29, 2006  R0.01  First stable version
  Jun 01, 2006  R0.02  Added FAT12. Removed unbuffered mode.
                       Fixed a problem on small (<32M) patition.
