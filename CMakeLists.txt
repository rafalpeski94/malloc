cmake_minimum_required(VERSION 3.0.0)
project(malloc_custom VERSION 0.1.0 LANGUAGES C)
add_executable(malloc_custom main.c malloc_custom.c)
target_compile_definitions(malloc_custom PUBLIC MALLOC_BUFF_SIZE=1024 MEMFREE_SET_ZERO)
