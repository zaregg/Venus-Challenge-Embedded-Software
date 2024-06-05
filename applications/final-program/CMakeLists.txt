cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)


# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -pthread -Wno-psabi")  # Compiler flags for C++ files
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -ggdb -O0 -pthread -D_POSIX_C_SOURCE=200809L")  # Compiler flags for C files

# Set source files using file(GLOB ...)
file(GLOB CPP_SOURCES
    "src/*.cpp"
    "src/threads/*.cpp"
    "src/threads/sensors/*.cpp"
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../../library
    ${CMAKE_SOURCE_DIR}/../../platform
    ${CMAKE_SOURCE_DIR}/../../external
)

# Link directories
link_directories(
    ${CMAKE_SOURCE_DIR}/../../build/lib
)

FIND_PACKAGE( Boost 1.74.0 COMPONENTS thread program_options REQUIRED )
# find_package(nlohmann_json 3.2.0 REQUIRED)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# Compile C++ source files
add_executable(main ${CPP_SOURCES})

# Find and include Boost
# find_package(Boost REQUIRED COMPONENTS lockfree)
# include_directories(${Boost_INCLUDE_DIRS})

# Link against necessary libraries (Boost.Lockfree and pthread)
target_link_libraries(main pthread ${CMAKE_SOURCE_DIR}/../../build/lib/libpynq.a Boost::boost)

# Specify cleaning rules
add_custom_target(clean-all COMMAND rm -rf ${CMAKE_BINARY_DIR}/* ${CMAKE_SOURCE_DIR}/main)

# Specify run rules
add_custom_target(run COMMAND sudo ./main)

# Set capabilities after build
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env VERBOSE=${CMAKE_VERBOSE_MAKEFILE} ${SUDO} setcap cap_sys_nice,cap_sys_rawio+ep $<TARGET_FILE:main>)
