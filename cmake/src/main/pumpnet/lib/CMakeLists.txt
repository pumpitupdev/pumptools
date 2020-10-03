project(pumpnet-lib)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/pumpnet/lib)

set(SOURCE_FILES
        ${SRC}/http.c
        ${SRC}/profile-token.c
        ${SRC}/pumpnet.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(${PROJECT_NAME} asset-nx2 util)
