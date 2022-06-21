project(ptapi-io-piuio-lxio)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/ptapi/io/piuio/lxio)

set(SOURCE_FILES
        ${SRC}/lxio.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")
# Remove library name "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} io-lxio io-util util usb-1.0)