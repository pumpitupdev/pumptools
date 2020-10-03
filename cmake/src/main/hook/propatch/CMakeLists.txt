project(propatch)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/hook/propatch)

set(SOURCE_FILES
        ${SRC}/usb-fix.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(${PROJECT_NAME} capnhook-hook capnhook-hooklib)
