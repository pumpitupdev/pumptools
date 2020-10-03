project(x2hook)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/hook/x2)

add_resources(DOG_KEY ${SRC} x2hook.dog.key)

set(SOURCE_FILES
        ${SRC}/main.c
        ${SRC}/ogl-fix.c
        ${SRC}/options.c
        ${SRC}/unlock.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${DOG_KEY})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} asset-x2 hook-core microdog34 patch ptapi-io-piuio-util util dl pthread)