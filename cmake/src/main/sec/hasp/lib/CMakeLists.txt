project(hasp)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/sec/hasp/lib)

set(SOURCE_FILES
        ${SRC}/object/client-id-req.c
        ${SRC}/object/client-id-resp.c
        ${SRC}/asn1.c
        ${SRC}/handler.c
        ${SRC}/server.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(${PROJECT_NAME} crypt)
