//
// Created by dingjing on 25-7-9.
//

#include "file-type.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>


static inline const char* get_file_name (const char* filePath)
{
    return strrchr(filePath, '/') + 1;
}

static inline bool check_is_java_source (const char* buffer)
{
    cint64 idx = 0;
    char buf[1024] = {0};
    char* fhT = (char*) buffer;
    do {
        idx = c_str_copy_line_buffer(fhT, buf, sizeof(buf));
        fhT = fhT + idx;
        if (c_str_has_prefix(buf, "import ") && c_str_has_suffix(buf, ";")) {
            return true;
        }
    } while (idx > 0);

    return false;
}

static inline bool check_is_zip (const char* buffer)
{
    if (!memcmp(buffer, ZIP_MAGIC, ZIP_MAGIC_SIZE)) {
        return true;
    }
    return false;
}

static inline bool check_is_rar4 (const char* buffer)
{
    if (!memcmp(buffer, RAR_4_MAGIC, RAR_4_MAGIC_SIZE)) {
        return true;
    }
    return false;
}

static inline bool check_is_rar5 (const char* buffer)
{
    if (!memcmp(buffer, RAR_5_MAGIC, RAR_5_MAGIC_SIZE)) {
        return true;
    }
    return false;
}

static inline bool check_is_elf (const char* buffer)
{
    if (!memcmp(buffer, ELF_MAGIC, ELF_MAGIC_SIZE)) {
        return true;
    }
    return false;
}


FileRet file_parse(const char* filename, FileTypeDetail* detail)
{
    C_RETURN_VAL_IF_FAIL(filename && detail, RET_ERROR_INVALID_ARGS);
    C_RETURN_VAL_IF_FAIL(!access(filename, F_OK), RET_ERROR_FILE_NOT_EXISTS);

    int fd = 0;
    cint64 fileSize = 0;
    void* fileHeader = NULL;
    FileRet ret = RET_SUCCESS;
    memset(detail, 0, sizeof(FileTypeDetail));

    {
        struct stat st;
        C_RETURN_VAL_IF_FAIL(stat(filename, &st) >= 0, RET_ERROR_STAT);
        if (S_ISBLK(st.st_mode)) {
            detail->fileType = FT_BLK_DEV;
            return RET_SUCCESS;
        }
        else if (S_ISFIFO(st.st_mode)) {
            detail->fileType = FT_FIFO;
            return RET_SUCCESS;
        }
        else if (S_ISLNK(st.st_mode)) {
            detail->fileType = FT_LINK;
            return RET_SUCCESS;
        }
        else if (S_ISDIR(st.st_mode)) {
            detail->fileType = FT_DIR;
            return RET_SUCCESS;
        }
        else if (S_ISCHR(st.st_mode)) {
            detail->fileType = FT_CHR_DEV;
            return RET_SUCCESS;
        }
        C_RETURN_VAL_IF_FAIL(S_ISREG(st.st_mode), RET_ERROR_FILE_TYPE);
        fileSize = st.st_size;
    }

    do {
        fd = open(filename, O_RDONLY);
        if (fd < 0) { ret = RET_ERROR_OPEN; break; }

        fileHeader = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
        if (fileHeader == MAP_FAILED) { ret = RET_ERROR; break; }

        const char* fileName = get_file_name(filename);
        if (!fileName) { ret = RET_ERROR_INVALID_ARGS; break; }

        if (c_str_has_suffix(fileName, ".java") && check_is_java_source(fileHeader)) {
            detail->fileType = FT_JAVA_SOURCE;
        }
        else if (c_str_has_suffix(fileName, ".zip") && check_is_zip(fileHeader)) {
            detail->fileType = FT_ZIP;
        }
        else if (c_str_has_suffix(filename, ".rar") && check_is_rar4(fileHeader)) {
            detail->fileType = FT_RAR4;
        }
        else if (c_str_has_suffix(filename, ".rar") && check_is_rar5(fileHeader)) {
            detail->fileType = FT_RAR5;
        }
        else if (
            c_str_has_suffix(filename, ".gz")
            || c_str_has_suffix(filename, ".tar")
            || c_str_has_suffix(filename, ".bz2")
            || c_str_has_suffix(filename, ".7z")
            || c_str_has_suffix(filename, ".xz")
            || c_str_has_suffix(filename, ".z")
            || c_str_has_suffix(filename, ".cab")
            || c_str_has_suffix(filename, ".arj")
            || c_str_has_suffix(filename, ".lzh")
            || c_str_has_suffix(filename, ".xar")
            || c_str_has_suffix(filename, ".pmd")
            || c_str_has_suffix(filename, ".deb")
            || c_str_has_suffix(filename, ".rpm")
            || c_str_has_suffix(filename, ".chm")
            || c_str_has_suffix(filename, ".iso")
            || c_str_has_suffix(filename, ".vhd")
            || c_str_has_suffix(filename, ".wim")
            || c_str_has_suffix(filename, ".mdf")
            || c_str_has_suffix(filename, ".nri")
            || c_str_has_suffix(filename, ".swf")
            || c_str_has_suffix(filename, ".mar")) {
            // detail->fileType = FT_ZIP;
        }
        else {
            // 其他扩展名
            if (check_is_elf(fileHeader)) {
                detail->fileType = FT_ELF;
            }
        }

    } while (false);

    C_FREE_FUNC_NULL(fileHeader, munmap, fileSize);
    C_FREE_FUNC_IF_OK(fd, fd >= 0, close);

    return ret;
}

const char* file_error_string(FileRet fileRet)
{
    if ((int) fileRet >= (int) RET_SUCCESS && fileRet < (int) RET_NUM) {
        return gFileRetStr[fileRet];
    }

    return "Unknown error";
}

const char* file_get_type_string(FileTypeDetail* detail)
{
    C_RETURN_VAL_IF_FAIL(detail, "Unknown file type");

    switch (detail->fileType) {
        case FT_CHR_DEV: {
            return "Char device";
        }
        case FT_FIFO: {
            return "fifo";
        }
        case FT_LINK: {
            return "Link file";
        }
        case FT_BLK_DEV: {
            return "Block device";
        }
        case FT_DIR: {
            return "Directory";
        }
        case FT_ELF: {
            return "ELF file";
        }
        case FT_ZIP: {
            return "Zip file";
        }
        case FT_RAR4: {
            return "RAR 4.x file";
        }
        case FT_RAR5: {
            return "RAR 5.x file";
        }
        case FT_JAVA_SOURCE: {
            return "Java Source file";
        }
        default: {
            break;
        }
    }

    return "Unknown file type";
}
