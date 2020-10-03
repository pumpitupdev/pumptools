include_directories(${PT_ROOT_IMPORTS})
include_directories(${PT_ROOT_PTAPI})
include_directories(${PT_ROOT_MAIN})
include_directories(${PT_ROOT_TEST})

add_subdirectory(capnhook)
add_subdirectory(hook)
add_subdirectory(test-util)
add_subdirectory(util)