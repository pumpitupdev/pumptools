project(test-hook-propatch-usb-fix)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_TEST}/hook/propatch/usb-fix)

set(SOURCE_FILES
        ${SRC}/main.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} cmocka propatch test-util util)
