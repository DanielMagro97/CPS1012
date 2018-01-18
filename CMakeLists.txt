cmake_minimum_required(VERSION 3.7)
project(CPS1012)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c)
add_executable(CPS1012 ${SOURCE_FILES})
target_link_libraries(CPS1012 ncurses)