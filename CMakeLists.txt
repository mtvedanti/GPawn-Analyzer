cmake_minimum_required(VERSION 3.10)
project(GPawn-Analyzer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)	
# 1. Define the executable and ONLY include source files (.cpp) that exist
add_executable(GPawn-Analyzer 
    src/main.cpp
)

# 2. Tell CMake where to find header files (like your header-only chess.hpp)
# The target name "GPawn-Analyzer" matches the line above exactly.
target_include_directories(GPawn-Analyzer PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/extern
)
