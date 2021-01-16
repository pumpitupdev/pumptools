project(test-util-base64)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_TEST}/util/base64)

set(SOURCE_FILES
        ${SRC}/main.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} cmocka test-util util)
