project(patch)
message(STATUS "Project " ${PROJECT_NAME})

set(SRC ${PT_ROOT_MAIN}/hook/patch)

set(SOURCE_FILES
        ${SRC}/asound-fix.c
        ${SRC}/amixer-block.c
        ${SRC}/blacklist-url.c
        ${SRC}/block-keyboard-grab.c
        ${SRC}/gfx.c
        ${SRC}/hasp.c
        ${SRC}/hdd-check.c
        ${SRC}/hook-mon.c
        ${SRC}/main-loop.c
        ${SRC}/microdog34.c
        ${SRC}/microdog40.c
        ${SRC}/mounts.c
        ${SRC}/net-profile.c
        ${SRC}/network.c
        ${SRC}/piubtn.c
        ${SRC}/piuio.c
        ${SRC}/piuio-exit.c
        ${SRC}/ram-wipe.c
        ${SRC}/redir.c
        ${SRC}/sigsegv.c
        ${SRC}/sound.c
        ${SRC}/usb-emu.c
        ${SRC}/usb-init-fix.c
        ${SRC}/usb-mnt.c
        ${SRC}/usb-unlock.c
        ${SRC}/usbfs.c
        ${SRC}/x11-event-loop.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(${PROJECT_NAME} capnhook-hook capnhook-hooklib hasp-old microdog34 microdog40)
