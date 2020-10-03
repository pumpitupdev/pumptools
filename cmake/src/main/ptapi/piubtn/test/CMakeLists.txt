project(ptapi-io-piubtn-test)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/ptapi/io/piubtn/test)

set(SOURCE_FILES
        ${SRC}/main.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} capnhook-hook ptapi-io-piubtn-util util)