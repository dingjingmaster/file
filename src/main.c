#include <stdio.h>

#include "file-type.h"


int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    FileTypeDetail detail;
    const FileRet ret = file_parse(argv[1], &detail);
    if (ret != RET_SUCCESS) {
        printf("Failed to parse file: %s, error: %s\n", argv[1], file_error_string(ret));
        return 2;
    }

    printf("File type: %s\n", file_get_type_string(&detail));

    return 0;
}
