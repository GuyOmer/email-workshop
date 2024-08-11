cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 23)
add_executable(mime2 mime2.c ../common/secrets.h)
target_link_libraries(mime2 curl)