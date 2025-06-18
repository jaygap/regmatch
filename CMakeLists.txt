cmake_minimum_required(VERSION 3.10.0)
project(regmatch VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_FLAGS "-O0 -ggdb")

add_executable(regmatch
    src/main.cpp
)

include_directories(include)

target_link_libraries(regmatch
)
