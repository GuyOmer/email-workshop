cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 23)
add_executable(smtp2 smtp2.c)
target_link_libraries(smtp2 curl)