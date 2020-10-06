project(nxahook)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/hook/nxa)

add_resources(HDD_RAW ${SRC} nxahook.hdd.raw)
add_resources(DOG_KEY ${SRC} nxahook.dog.key)

set(SOURCE_FILES
        ${SRC}/main.c
        ${SRC}/options.c
        ${SRC}/profile-gen.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HDD_RAW} ${DOG_KEY})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} asset-nxa crypt hook-core microdog40 asset-nxa patch ptapi-io-piuio-util pumpnet-lib util dl pthread curl)