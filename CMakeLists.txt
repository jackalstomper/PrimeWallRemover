cmake_minimum_required(VERSION 3.13 FATAL_ERROR) # 3.13 because nod uses TARGETS in ways that are broken before .13
project(PrimeWallRemover VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    # Nod needs these or it fails to build
    add_compile_definitions(_WIN32 UNICODE)
endif()

add_subdirectory(nod)

add_executable(PrimeWallRemover
    src/main.cpp
    src/Common.cpp
    src/WallRemover.cpp
    src/prime_files/PAK.cpp)

target_link_libraries(PrimeWallRemover nod)
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
    target_link_libraries(PrimeWallRemover stdc++fs)
endif()
