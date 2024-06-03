cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Set compiler flags
set(CMAKE_CXX_STANDARD 11)  # Set C++ standard to C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -pthread")  # Compiler flags for C++ files
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -ggdb -O0 -pthread -D_POSIX_C_SOURCE=200809L")  # Compiler flags for C files

# Set source files using file(GLOB ...)
file(GLOB CPP_SOURCES
    "src/*.cpp"
    "src/threads/*.cpp"
    "src/threads/sensors/*.cpp"
)

# Compile C++ source filessu
add_executable(main ${CPP_SOURCES})

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../../library
    ${CMAKE_SOURCE_DIR}/../../platform
    ${CMAKE_SOURCE_DIR}/../../external
)

link_directories(
    ${CMAKE_SOURCE_DIR}/../../library
    ${CMAKE_SOURCE_DIR}/../../platform
    ${CMAKE_SOURCE_DIR}/../../external
    ${CMAKE_SOURCE_DIR}/../../build/lib
)

# Link against necessary libraries
target_link_libraries(main pthread ${CMAKE_SOURCE_DIR}/../../build/lib/libpynq.a)

# Specify cleaning rules
add_custom_target(clean-all COMMAND rm -rf ${CMAKE_BINARY_DIR}/* ${CMAKE_SOURCE_DIR}/main)
add_custom_target(run COMMAND sudo ./main)
add_custom_command(TARGET main POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E env VERBOSE=${CMAKE_VERBOSE_MAKEFILE} ${SUDO} setcap cap_sys_nice,cap_sys_rawio+ep $<TARGET_FILE:main>)
