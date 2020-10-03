project(capnhook-hook)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/capnhook/hook)

set(SOURCE_FILES
        ${PT_ROOT_MAIN}/capnhook/hook/fshook.c
        ${PT_ROOT_MAIN}/capnhook/hook/filehook.c
        ${PT_ROOT_MAIN}/capnhook/hook/iobuf.c
        ${PT_ROOT_MAIN}/capnhook/hook/iohook.c
        ${PT_ROOT_MAIN}/capnhook/hook/lib.c
        ${PT_ROOT_MAIN}/capnhook/hook/result.c
        ${PT_ROOT_MAIN}/capnhook/hook/sig.c
        ${PT_ROOT_MAIN}/capnhook/hook/usbhook.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(${PROJECT_NAME} dl util)