cmake_minimum_required(VERSION 3.10)
project(problem_053)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra")

add_executable(code main.cpp)

# Include the current directory for headers
target_include_directories(code PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})