cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 23)
add_executable(mime2 mime2.c)
add_compile_options(-O0 -ggdb -Werror -pedantic -Wall)
target_link_libraries(mime2 curl)