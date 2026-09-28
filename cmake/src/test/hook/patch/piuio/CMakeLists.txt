project(test-hook-patch-piuio)
message(STATUS "Project " ${PROJECT_NAME})

set(TEST_SRC ${PT_ROOT_TEST}/hook/patch/piuio)
set(PATCH_SRC ${PT_ROOT_MAIN}/hook/patch)

set(SOURCE_FILES
        ${TEST_SRC}/main.c
        ${PATCH_SRC}/piuio.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} cmocka util)
