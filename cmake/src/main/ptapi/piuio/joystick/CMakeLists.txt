project(ptapi-io-piuio-joystick)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/ptapi/io/piuio/joystick)

set(SOURCE_FILES
        ${SRC}/joystick.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")
# Remove library name "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} ptapi-io-piuio-joystick-util io-util util)