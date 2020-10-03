project(prihook)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/hook/pri)

add_resources(HDD_RAW ${SRC} prihook.hdd.raw)
add_resources(DOG_KEY ${SRC} prihook.hasp.key)

set(SOURCE_FILES
        ${SRC}/main.c
        ${SRC}/options.c
        ${SRC}/usb-updates.c)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HDD_RAW} ${DOG_KEY})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} crypt hasp-old hook-core patch ptapi-io-piuio-util util dl pthread)