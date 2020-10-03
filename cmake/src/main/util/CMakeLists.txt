project(util)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/util)

set(SOURCE_FILES
        ${SRC}/adler32.c
        ${SRC}/array.c
        ${SRC}/base64.c
        ${SRC}/fs.c
        ${SRC}/glibc.c
        ${SRC}/hex.c
        ${SRC}/iobuf.c
        ${SRC}/list.c
        ${SRC}/log.c
        ${SRC}/mem.c
        ${SRC}/net.c
        ${SRC}/options.c
        ${SRC}/patch.c
        ${SRC}/proc.c
        ${SRC}/rand.c
        ${SRC}/sock-tcp.c
        ${SRC}/str.c
        ${SRC}/sys-info.c
        ${SRC}/test.c
        ${SRC}/time.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
