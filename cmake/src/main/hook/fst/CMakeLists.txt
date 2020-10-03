project(fsthook)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/hook/fst)

add_resources(HDD_RAW ${SRC} fsthook.hdd.raw)
add_resources(DOG_KEY ${SRC} fsthook.dog.key)

set(SOURCE_FILES
        ${SRC}/main.c
        ${SRC}/options.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HDD_RAW} ${DOG_KEY})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} crypt hook-core microdog40 patch ptapi-io-piuio-util util dl pthread)