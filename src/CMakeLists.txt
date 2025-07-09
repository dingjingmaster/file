add_library(file-type STATIC file-type.c file-type.h)

add_executable(file main.c)
target_link_libraries(file PRIVATE file-type)