cmake_minimum_required(VERSION 3.0)
project(email C)

set(CMAKE_C_STANDARD 23)

add_subdirectory(smtp)
add_subdirectory(mime)
add_subdirectory(pop3)
add_subdirectory(imap)