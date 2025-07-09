//
// Created by dingjing on 25-7-9.
//

#ifndef file_FILE_TYPE_H
#define file_FILE_TYPE_H
#include "macros/macros.h"


C_BEGIN_EXTERN_C

typedef enum
{
    RET_SUCCESS = 0,
    RET_ERROR,
    RET_ERROR_STAT,
    RET_ERROR_OPEN,
    RET_ERROR_FILE_TYPE,
    RET_ERROR_INVALID_ARGS,
    RET_ERROR_FILE_NOT_EXISTS,
    RET_NUM,
} FileRet;

static const char* gFileRetStr[] = {
    "Success",
    "Unknown error",
    "Stat error",
    "Open file error",
    "Error file type",
    "Invalid input parameter",
    "File does not exist",
    NULL,
};

typedef enum
{
    FT_UNKNOWN = 0,
    FT_DIR,
    FT_FIFO,
    FT_LINK,
    FT_EMPTY,
    FT_CHR_DEV,
    FT_BLK_DEV,

    FT_Z,
    FT_GZ,
    FT_XZ,
    FT_7Z,
    FT_ARJ,
    FT_BZ2,
    FT_CAB,
    FT_CHM,
    FT_DEB,
    FT_ELF,
    FT_ISO,
    FT_JAR,
    FT_LZH,
    FT_PMD,
    FT_RPM,
    FT_TAR,
    FT_VHD,
    FT_WIM,
    FT_XAR,
    FT_ZIP,

    FT_RAR4,
    FT_RAR5,

    FT_CLASS,

    FT_SWF_PURE,
    FT_SWF_ZLIB,
    FT_SWF_LZMA,

    FT_MAR_100,
    FT_MAR_120,
    FT_MAR_130,

    FT_JAVA_SOURCE,
} FileType;

typedef struct
{
    FileType                        fileType;       // 文件类型
    union {

    } detail;
} FileTypeDetail;


/* elf - start */
#define ELF_MAGIC                   "\177ELF"
#define ELF_MAGIC_SIZE              4
/* elf - end   */

/* zip - start */
#define ZIP_MAGIC                   "PK\003\004"
#define ZIP_MAGIC_SIZE              4
/* zip - stop  */

/* rar - start */
#define RAR_4_MAGIC                 "Rar!\032\007\000"
#define RAR_4_MAGIC_SIZE            7

#define RAR_5_MAGIC                 "Rar!\032\007\001\000"
#define RAR_5_MAGIC_SIZE            8
/* rar - stop */

/* gz - start */
#define GZ_MAGIC                    "\037\213"
#define GZ_MAGIC_SIZE               2
/* gz - stop */

/* xz - start */
#define XZ_MAGIC                    "\3757zXZ\000"
#define XZ_MAGIC_SIZE               6
/* xz - stop */

/* 7z - start */
#define Z7_MAGIC                    "7z\274\257'\034"
#define Z7_MAGIC_SIZE               6
/* 7z - stop */

/* tar - start */
#define TAR_MAGIC                   "ustar"
#define TAR_MAGIC_SIZE              5
#define TAR_MAGIC_OFFSET            257
/* tar - stop */

/* bz2 - start */
#define BZ2_MAGIC                   "BZh"
#define BZ2_MAGIC_SIZE              3
/* bz2 - stop */

/* Z - start */
#define Z_MAGIC                     "\037\235"
#define Z_MAGIC_SIZE                2
/* Z - stop */

/* cab - start */
#define CAB_MAGIC                   "MSCF"
#define CAB_MAGIC_SIZE              4
/* cab - stop */

/* arj - start */
#define ARJ_MAGIC                   "`\352"
#define ARJ_MAGIC_SIZE              2
/* arj - stop */

/* lzh - start */
#define LZH_MAGIC                   "-lh5-"
#define LZH_MAGIC_SIZE              5
/* lzh - stop */

/* xar - start */
#define XAR_MAGIC                   "xar!"
#define XAR_MAGIC_SIZE              4
/* xar - stop */

/* deb - start */
#define DEB_MAGIC                   "!<arch>\n"
#define DEB_MAGIC_SIZE              8
/* deb - stop */

/* pmd - start */
#define PMD_MAGIC                   "PMD "
#define PMD_MAGIC_SIZE              4
/* pmd - stop */

/* rpm - start */
#define RPM_MAGIC                   "\355\253\356\333"
#define RPM_MAGIC_SIZE              4
/* rpm - stop */

/* chm - start */
#define CHM_MAGIC                   "ITSF"
#define CHM_MAGIC_SIZE              4
/* chm - stop */

/* iso - start */
#define ISO_MAGIC                   "CD001"
#define ISO_MAGIC_SIZE              5
#define ISO_MAGIC_OFFSET            32769
/* iso - stop */

/* vhd - start */
#define VHD_MAGIC                   "conectix"
#define VHD_MAGIC_SIZE              8
/* vhd - stop */

/* wim - start */
#define WIM_MAGIC                   "MSWIM\0\0\0"
#define WIM_MAGIC_SIZE              8
/* wim - stop */

/* mdf - start */
#define MDF_MAGIC                   "MSWIM\0\0\0"
#define MDF_MAGIC_SIZE              8
/* mdf - stop */

/* swf - start */
#define SWF_MAGIC_PURE              "FWS"
#define SWF_MAGIC_ZLIB              "CWS"
#define SWF_MAGIC_LZMA              "ZWS"
#define SWF_MAGIC_SIZE              8
/* swf - stop */

/* mar - start */
#define MAR_MAGIC_100               "MAR100"
#define MAR_MAGIC_120               "MAR120"
#define MAR_MAGIC_130               "MAR130"
#define MAR_MAGIC_SIZE              6
/* mar - stop */

/* jar - start */
#define JAR_MAGIC                   "PK\003\004\024\000\010\000\010\000"
#define JAR_MAGIC_SIZE              10
/* jar - end   */

/* class - start */
#define CLASS_MAGIC                 "\312\376\272\276"
#define CLASS_MAGIC_SIZE            4
/* class - end   */

FileRet         file_parse              (C_IN const char *filename, C_OUT FileTypeDetail *detail);
const char*     file_error_string       (C_IN FileRet fileRet);
const char*     file_get_type_string    (C_IN FileTypeDetail* detail);


C_END_EXTERN_C

#endif // file_FILE_TYPE_H
