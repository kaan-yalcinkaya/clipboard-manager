cmake_minimum_required(VERSION 3.5)
project(Clipboard-Manager C)
file(GLOB SRC "source/*.c")
add_compile_options(/utf-8)
add_executable(Clipboard-Manager main.c ${SRC})
target_include_directories(Clipboard-Manager PUBLIC include)
