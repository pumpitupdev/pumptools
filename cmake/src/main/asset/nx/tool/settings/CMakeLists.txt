project(nx-settings-tool)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/asset/nx/tool)

set(SOURCE_FILES
        ${SRC}/settings.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} asset-nx util)
