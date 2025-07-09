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

static inline bool check_is_magic (const char* buffer, cint64 bufferLen, const char* magic, int32_t magicLen, cint64 offset)
{
    C_RETURN_VAL_IF_FAIL(buffer && magic && offset + magicLen < bufferLen, false);

    if (!memcmp(buffer + offset, magic, magicLen)) {
        return true;
    }
    return false;
}

static inline bool check_is_magic_vhd (const char* buffer, cint64 bufferLen)
{
    C_RETURN_VAL_IF_FAIL(buffer && bufferLen > 512, false);

    if (!memcmp(buffer + bufferLen - 512, VHD_MAGIC, VHD_MAGIC_SIZE)) {
        return true;
    }
    return false;
}

static inline bool check_is_jar (const char* buffer, cint64 bufferLen)
{
    C_RETURN_VAL_IF_FAIL(buffer, false);

    static const int jarFlagFin = 1024;
    static const char* jarFlag = "META-INF/MANIFEST.MF";
    const int jarFlagLen = (int) strlen (jarFlag);
    if (check_is_magic(buffer, bufferLen, ZIP_MAGIC, ZIP_MAGIC_SIZE, 0)) {
        for (int i = 30; i < jarFlagFin; i++) {
            if (!memcmp(buffer + i, jarFlag, jarFlagLen)) {
                return true;
            }
        }
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
        if (fileSize == 0) {
            detail->fileType = FT_EMPTY;
            return RET_SUCCESS;
        }
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
        else if (check_is_jar(fileHeader, fileSize)) {
            detail->fileType = FT_JAR;
        }
        else if (check_is_magic(fileHeader, fileSize, ZIP_MAGIC, ZIP_MAGIC_SIZE, 0)) {
            detail->fileType = FT_ZIP;
        }
        else if (check_is_magic(fileHeader, fileSize, RAR_4_MAGIC, RAR_4_MAGIC_SIZE, 0)) {
            detail->fileType = FT_RAR4;
        }
        else if (check_is_magic(fileHeader, fileSize, RAR_5_MAGIC, RAR_5_MAGIC_SIZE, 0)) {
            detail->fileType = FT_RAR5;
        }
        else if (check_is_magic(fileHeader, fileSize, GZ_MAGIC, GZ_MAGIC_SIZE, 0)) {
            detail->fileType = FT_GZ;
        }
        else if (check_is_magic(fileHeader, fileSize, XZ_MAGIC, XZ_MAGIC_SIZE, 0)) {
            detail->fileType = FT_XZ;
        }
        else if (check_is_magic(fileHeader, fileSize, Z7_MAGIC, Z7_MAGIC_SIZE, 0)) {
            detail->fileType = FT_7Z;
        }
        else if (check_is_magic(fileHeader, fileSize, TAR_MAGIC, TAR_MAGIC_SIZE, TAR_MAGIC_OFFSET)) {
            detail->fileType = FT_TAR;
        }
        else if (check_is_magic(fileHeader, fileSize, BZ2_MAGIC, BZ2_MAGIC_SIZE, 0)) {
            detail->fileType = FT_BZ2;
        }
        else if (check_is_magic(fileHeader, fileSize, Z_MAGIC, Z_MAGIC_SIZE, 0)) {
            detail->fileType = FT_Z;
        }
        else if (check_is_magic(fileHeader, fileSize, CAB_MAGIC, CAB_MAGIC_SIZE, 0)) {
            detail->fileType = FT_CAB;
        }
        else if (check_is_magic(fileHeader, fileSize, ARJ_MAGIC, ARJ_MAGIC_SIZE, 0)) {
            detail->fileType = FT_ARJ;
        }
        else if (check_is_magic(fileHeader, fileSize, LZH_MAGIC, LZH_MAGIC_SIZE, 0)) {
            detail->fileType = FT_LZH;
        }
        else if (check_is_magic(fileHeader, fileSize, XAR_MAGIC, XAR_MAGIC_SIZE, 0)) {
            detail->fileType = FT_XAR;
        }
        else if (check_is_magic(fileHeader, fileSize, DEB_MAGIC, DEB_MAGIC_SIZE, 0)) {
            detail->fileType = FT_DEB;
        }
        else if (check_is_magic(fileHeader, fileSize, PMD_MAGIC, PMD_MAGIC_SIZE, 0)) {
            detail->fileType = FT_PMD;
        }
        else if (check_is_magic(fileHeader, fileSize, RPM_MAGIC, RPM_MAGIC_SIZE, 0)) {
            detail->fileType = FT_RPM;
        }
        else if (check_is_magic(fileHeader, fileSize, CHM_MAGIC, CHM_MAGIC_SIZE, 0)) {
            detail->fileType = FT_CHM;
        }
        else if (check_is_magic(fileHeader, fileSize, ISO_MAGIC, ISO_MAGIC_SIZE, ISO_MAGIC_OFFSET)) {
            detail->fileType = FT_ISO;
        }
        else if (check_is_magic_vhd(fileHeader, fileSize)) {
            detail->fileType = FT_VHD;
        }
        else if (check_is_magic(fileHeader, fileSize, WIM_MAGIC, WIM_MAGIC_SIZE, 0)) {
            detail->fileType = FT_WIM;
        }
        else if (check_is_magic(fileHeader, fileSize, SWF_MAGIC_PURE, SWF_MAGIC_SIZE, 0)) {
            detail->fileType = FT_SWF_PURE;
        }
        else if (check_is_magic(fileHeader, fileSize, SWF_MAGIC_ZLIB, SWF_MAGIC_SIZE, 0)) {
            detail->fileType = FT_SWF_ZLIB;
        }
        else if (check_is_magic(fileHeader, fileSize, SWF_MAGIC_LZMA, SWF_MAGIC_SIZE, 0)) {
            detail->fileType = FT_SWF_LZMA;
        }
        else if (check_is_magic(fileHeader, fileSize, MAR_MAGIC_100, MAR_MAGIC_SIZE, 0)) {
            detail->fileType = FT_MAR_100;
        }
        else if (check_is_magic(fileHeader, fileSize, MAR_MAGIC_120, MAR_MAGIC_SIZE, 0)) {
            detail->fileType = FT_MAR_120;
        }
        else if (check_is_magic(fileHeader, fileSize, MAR_MAGIC_130, MAR_MAGIC_SIZE, 0)) {
            detail->fileType = FT_MAR_130;
        }
        else {
            // 其他扩展名
            if (check_is_magic(fileHeader, fileSize, ELF_MAGIC, ELF_MAGIC_SIZE, 0)) {
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
        case FT_EMPTY: {
            return "Empty file";
        }
        case FT_ELF: {
            return "ELF file";
        }
        case FT_GZ: {
            return "GZ file";
        }
        case FT_XZ: {
            return "XZ file";
        }
        case FT_7Z: {
            return "7-Zip file";
        }
        case FT_CAB: {
            return "Microsoft Cabinet file";
        }
        case FT_ARJ: {
            return "ARJ(Archived by Robert Jung) file";
        }
        case FT_LZH: {
            return "LZH(Lempel-Ziv-Haruyasu) file";
        }
        case FT_XAR: {
            return "XAR(Extensible Archive Format) file";
        }
        case FT_DEB: {
            return "Debian-binary deb file";
        }
        case FT_PMD: {
            return "PMD(Polygon Model Data) file";
        }
        case FT_RPM: {
            return "RPM(Red Hat Package Manager) File";
        }
        case FT_CHM: {
            return "CHM(Compiled HTML Help) File";
        }
        case FT_Z: {
            return "Z compress'd file";
        }
        case FT_ISO: {
            return "ISO 9660 file";
        }
        case FT_VHD: {
            return "VHD(Virtual Hard Disk) file";
        }
        case FT_WIM: {
            return "WIM(Windows Imaging Format) file";
        }
        case FT_SWF_PURE: {
            return "SWF no compress file";
        }
        case FT_SWF_ZLIB: {
            return "SWF zlib compress'd file";
        }
        case FT_SWF_LZMA: {
            return "SWF lzma compress'd file";
        }
        case FT_MAR_100: {
            return "MAR 100";
        }
        case FT_MAR_120: {
            return "MAR 120";
        }
        case FT_MAR_130: {
            return "MAR 130";
        }
        case FT_JAR: {
            return "JAR(Java Archive) file";
        }
        case FT_TAR: {
            return "TAR file";
        }
        case FT_BZ2: {
            return "bzip2 file";
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
