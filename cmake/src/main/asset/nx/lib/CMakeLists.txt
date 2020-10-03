project(asset-nx)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/asset/nx/lib)

set(SOURCE_FILES
        ${SRC}/settings.c
        ${SRC}/util.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
