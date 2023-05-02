cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 23)

set(SRC_PROJECT_NAME restfs)
set(SRC_PROJECT_VERSION 0.0.0)

# This project uses vcpkg. We must then point to
# the corresponding toolchain file used so CMake
# can find dependencies.
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}scripts/buildsystems/vcpkg.cmake)

project(${SRC_PROJECT_NAME} VERSION ${SRC_PROJECT_VERSION})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

# Include these packages for build.
# We are putting these 'find' calls here to make
# it easier to identify what dependencies are
# active in this project.
find_package(restclient-cpp CONFIG REQUIRED)
find_package(FUSE REQUIRED)

include_directories(${FUSE_INCLUDE_DIR})

# Include this project application.
add_subdirectory(src)
