project(ptapi-io-piuio-joystick-util)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/ptapi/io/piuio/joystick-util)

set(SOURCE_FILES
        ${SRC}/joystick-conf.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")

target_link_libraries(${PROJECT_NAME} io-util util)