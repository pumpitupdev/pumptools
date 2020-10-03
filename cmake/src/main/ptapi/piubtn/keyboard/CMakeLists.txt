project(ptapi-io-piubtn-keyboard)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/ptapi/io/piubtn/keyboard)

set(SOURCE_FILES
        ${SRC}/keyboard.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")
# Remove library name "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} ptapi-io-piubtn-keyboard-util io-util util)