#include <stdio.h>

#include "file-type.h"


int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        FileTypeDetail detail;
        const FileRet ret = file_parse(argv[i], &detail);
        if (ret != RET_SUCCESS) {
            printf("Failed to parse file: %s, error: %s\n", argv[i], file_error_string(ret));
            return 2;
        }
        printf("File: '%s' File type: %s\n", argv[i], file_get_type_string(&detail));
    }

    return 0;
}
