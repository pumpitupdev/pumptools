project(io-util)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/io/util)

set(SOURCE_FILES
        ${SRC}/joystick.c
        ${SRC}/joystick-util.c
        ${SRC}/usb.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})