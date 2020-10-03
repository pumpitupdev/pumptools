project(crypt)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/crypt)

set(SOURCE_FILES
        ${SRC}/lib/md5.c
        ${SRC}/lib/rijndael.c
        ${SRC}/aes.c
        ${SRC}/md5.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
