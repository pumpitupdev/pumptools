project(microdog40d)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/sec/microdog40)

set(SOURCE_FILES
        ${SRC}/main.c
        ${SRC}/microdog40d.c
        ${SRC}/microdog40.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} crypt util pthread)
