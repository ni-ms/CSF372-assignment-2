cmake_minimum_required(VERSION 3.24)
project(OS_assignment_final C)

set(CMAKE_C_STANDARD 11)

add_executable(OS_assignment_final main.c)
target_link_libraries(OS_assignment_final -pthread)
