project(f2hook)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/hook/f2)

add_resources(HDD_RAW ${SRC} f2hook.hdd.raw)
add_resources(DOG_KEY ${SRC} f2hook.hasp.key)

set(SOURCE_FILES
        ${SRC}/main.c
        ${SRC}/options.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HDD_RAW} ${DOG_KEY})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-shared")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} crypt hook-core microdog34 patch ptapi-io-piuio-util util dl pthread)