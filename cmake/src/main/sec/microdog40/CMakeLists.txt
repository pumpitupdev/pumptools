project(microdog40)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/sec/microdog40)

set(SOURCE_FILES
        ${SRC}/microdog40.c
        ${SRC}/microdog40d.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(${PROJECT_NAME} crypt)
