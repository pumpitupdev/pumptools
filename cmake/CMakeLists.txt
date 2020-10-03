cmake_minimum_required(VERSION 2.8)

option(PT_PUBLIC_RELEASE "PT_PUBLIC_RELEASE" "0")

if (PT_PUBLIC_RELEASE STREQUAL "1")
    message(WARNING "[1;31m!!! Compiling public release !!!\n!!! Compiling public release !!!\n!!! Compiling public release !!![m")
endif ()

# set git revision
execute_process(COMMAND git log -1 --format=%h --date=short HEAD WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_VARIABLE GITREV ERROR_QUIET)
string(STRIP "${GITREV}" GITREV)
if (GITREV STREQUAL "")
    set(GITREV "not available")
endif ()

include(modules/AddResources.cmake)

set(PT_ROOT_SRC "${CMAKE_CURRENT_LIST_DIR}/../src")

set(PT_ROOT_IMPORTS "${PT_ROOT_SRC}/imports")
set(PT_ROOT_PTAPI "${PT_ROOT_SRC}/api")
set(PT_ROOT_MAIN "${PT_ROOT_SRC}/main")
set(PT_ROOT_TEST "${PT_ROOT_SRC}/test")

# force out of source build
message(STATUS "Force out of source build check...")
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource)

if (insource)
    message(FATAL_ERROR "Do not build in your source dir. Use a dedicated build folder. Now go cleanup the cache in the CMakeCache.txt and CMakeFiles folder which don't belong there!")
endif ()

# binary output dir
set(PT_BIN_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE STRING "Output path for compiled binaries")

# For generic no-config case
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PT_BIN_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PT_BIN_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PT_BIN_OUTPUT_PATH})

# Common C/C++ save
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

# TODO cleanup flags and add them to each module separately
# Important note: Don't use -O3 because that will break the dongle emulation code for microdog40 (wtf)
# Also, -O3 seems to optimize too much stuff and kills the amixer-block module entirely (the system() preload
# can't be called anymore)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGITREV=${GITREV} -pipe -Wall -Werror -std=gnu99 -D_GNU_SOURCE -Wno-attributes -m32")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined -m32")

# Enable debug flags on non public builds
if (PT_PUBLIC_RELEASE STREQUAL "0")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -g -rdynamic")
endif ()

# Disable asserts and strip binary
if (PT_PUBLIC_RELEASE STREQUAL "1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -DPT_PUBLIC_RELEASE")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -s")
endif ()

add_subdirectory(src)

# Add a text file containing the git commit rev
file(WRITE ${CMAKE_BINARY_DIR}/version ${GITREV})