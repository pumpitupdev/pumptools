project(test-util-str)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_TEST}/util/str)

set(SOURCE_FILES
        ${SRC}/main.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} cmocka test-util util)
