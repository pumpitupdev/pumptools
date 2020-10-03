project(ptapi-io-piubtn-keyboard-conf)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/ptapi/io/piubtn/keyboard-conf)

set(SOURCE_FILES
        ${SRC}/main.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ptapi-io-piubtn-keyboard-util io-util util X11)