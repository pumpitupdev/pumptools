project(pumpnet-prinet-proxy)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/pumpnet/prinet-proxy)

add_resources(PRI_PRIVATE_KEY ${SRC} prime.private.key)
add_resources(PRI_PUBLIC_KEY ${SRC} prime.public.key)

set(SOURCE_FILES
        ${SRC}/main.c
        ${SRC}/options.c
        ${SRC}/packet.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${PRI_PRIVATE_KEY} ${PRI_PUBLIC_KEY})

target_link_libraries(${PROJECT_NAME} pumpnet-lib sec-prinet util)