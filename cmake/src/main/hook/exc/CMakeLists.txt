project(exchook)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/hook/exc)

set(SOURCE_FILES
        ${SRC}/eeprom.c
        ${SRC}/io.c
        ${SRC}/lockchip.c
        ${SRC}/main.c
        ${SRC}/mempatch.c
        ${SRC}/options.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} hook-core lockchip ptapi-io-piuio-util patch util dl pthread)