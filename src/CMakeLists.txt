file(GLOB_RECURSE reactor_src "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(reactor_share SHARED ${reactor_src})