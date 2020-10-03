project(zerohook)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/hook/zero)

add_resources(HDD_RAW ${SRC} zerohook.hdd.raw)
add_resources(DOG_KEY ${SRC} zerohook.dog.key)

set(SOURCE_FILES
        ${SRC}/main.c
        ${SRC}/options.c
        ${SRC}/unlock.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${DOG_KEY} ${HDD_RAW})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} asset-zero hook-core microdog34 patch ptapi-io-piuio-util util dl pthread)