project(ptapi-io-piuio-null)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/ptapi/io/piuio/null)

set(SOURCE_FILES
        ${SRC}/null.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")
# Remove library name "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} io-util util)