project(capnhook-hooklib)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/capnhook/hooklib)

set(SOURCE_FILES
        ${PT_ROOT_MAIN}/capnhook/hooklib/filehook-mon.c
        ${PT_ROOT_MAIN}/capnhook/hooklib/fileopen-mon.c
        ${PT_ROOT_MAIN}/capnhook/hooklib/fshook-mon.c
        ${PT_ROOT_MAIN}/capnhook/hooklib/iohook-mon.c
        ${PT_ROOT_MAIN}/capnhook/hooklib/redir.c
        ${PT_ROOT_MAIN}/capnhook/hooklib/usb-emu.c
        ${PT_ROOT_MAIN}/capnhook/hooklib/usb-init-fix.c
        ${PT_ROOT_MAIN}/capnhook/hooklib/usbhook-mon.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(${PROJECT_NAME} capnhook-hook util)