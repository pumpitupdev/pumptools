project(test-capnhook-hook-lib)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_TEST}/capnhook/hook/lib)

set(SOURCE_FILES
        ${SRC}/main.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} capnhook-hooklib cmocka util dl)
