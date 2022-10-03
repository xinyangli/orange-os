cmake_minimum_required(VERSION 3.22)
project(orange_os C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(orange_os
        include/mm.h
        include/stddef.h
        include/types.h
        src/page_alloc.c)
