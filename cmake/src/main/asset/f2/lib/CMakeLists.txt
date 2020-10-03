project(asset-f2)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/asset/f2/lib)

set(SOURCE_FILES
        ${SRC}/usb-rank.c
        ${SRC}/usb-save.c
        ${SRC}/util.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
