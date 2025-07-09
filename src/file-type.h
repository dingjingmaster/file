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
    FT_CHR_DEV,
    FT_BLK_DEV,
    FT_FIFO,
    FT_LINK,
    FT_ELF,
    FT_ZIP,
    FT_RAR4,
    FT_RAR5,
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

/* jar - start */
#define JAR_MAGIC                   ""
#define JAR_MAGIC_SIZE              4
/* jar - end   */

FileRet         file_parse              (C_IN const char *filename, C_OUT FileTypeDetail *detail);
const char*     file_error_string       (C_IN FileRet fileRet);
const char*     file_get_type_string    (C_IN FileTypeDetail* detail);


C_END_EXTERN_C

#endif // file_FILE_TYPE_H
