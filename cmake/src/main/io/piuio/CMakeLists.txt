project(io-piuio)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/io/piuio)

set(SOURCE_FILES
        ${SRC}/device.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(piuio SHARED ${SOURCE_FILES})

set_target_properties(piuio PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} io-util util usb-1.0)