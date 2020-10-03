project(cmocka)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_SRC}/imports/cmocka)

include_directories(${SRC})

MESSAGE(STATUS, "${PT_IMPORTS_SRC}")

set(SOURCE_FILES
        ${SRC}/cmocka.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
