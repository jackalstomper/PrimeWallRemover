cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(PrimeWallRemover VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PrimeWallRemover_SOURCE_DIR}/build)
set(BUILD_SHARED_LIBRARIES OFF)

if (MSVC)
    add_definitions(-D_WIN32)
    add_definitions(-DUNICODE)
endif()

add_subdirectory(nod)
include_directories(nod/include)
include_directories(nod/logvisor/include)

add_executable(PrimeWallRemover
    src/main.cpp
    src/Common.cpp
    src/WallRemover.cpp
    src/prime_files/PAK.cpp
)

target_link_libraries(PrimeWallRemover nod)
target_link_libraries(PrimeWallRemover logvisor)
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
    target_link_libraries(PrimeWallRemover stdc++fs)
endif()
