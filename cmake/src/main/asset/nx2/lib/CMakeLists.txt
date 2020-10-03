project(asset-nx2)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/asset/nx2/lib)

set(SOURCE_FILES
        ${SRC}/usb-rank.c
        ${SRC}/usb-save.c
        ${SRC}/util.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} util)
