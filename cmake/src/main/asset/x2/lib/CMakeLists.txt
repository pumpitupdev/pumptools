project(asset-x2)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/asset/x2/lib)

set(SOURCE_FILES
        ${SRC}/settings.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
