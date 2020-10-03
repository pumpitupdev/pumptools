project(nx2-profile-tool)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/asset/nx2/tool)

set(SOURCE_FILES
        ${SRC}/usb-profile.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} asset-nx2 util)
