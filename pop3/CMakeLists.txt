cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 23)
add_executable(pop3 pop3.c)
target_link_libraries(pop3 curl)