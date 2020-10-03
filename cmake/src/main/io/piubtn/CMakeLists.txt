project(io-piubtn)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/io/piubtn)

set(SOURCE_FILES
        ${SRC}/device.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(piubtn SHARED ${SOURCE_FILES})

set_target_properties(piubtn PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} io-util util usb-1.0)