project(test-hook-patch-asound-fix)
message(STATUS "Project " ${PROJECT_NAME})

set(TEST_SRC ${PT_ROOT_TEST}/hook/patch/asound-fix)
set(PATCH_SRC ${PT_ROOT_MAIN}/hook/patch)

set(SOURCE_FILES
        ${TEST_SRC}/main.c
        ${PATCH_SRC}/asound-fix.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES LINK_FLAGS "-Wl,--wrap=fopen -Wl,--wrap=fclose")

target_link_libraries(${PROJECT_NAME} cmocka capnhook-hook util dl)
