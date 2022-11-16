cmake_minimum_required(VERSION 3.23)
project(OS_assignment_2 C)

set(CMAKE_C_STANDARD 99)

add_executable(OS_assignment_2 main.c)
target_link_libraries(OS_assignment_2 -pthread)