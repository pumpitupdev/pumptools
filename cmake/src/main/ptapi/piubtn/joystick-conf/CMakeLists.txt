project(ptapi-io-piubtn-joystick-conf)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/ptapi/io/piubtn/joystick-conf)

set(SOURCE_FILES
        ${SRC}/main.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ptapi-io-piubtn-joystick-util io-util util)