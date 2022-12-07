cmake_minimum_required(VERSION 3.4.1)

project(memalloc)

add_subdirectory(core)
add_subdirectory(utils)

add_executable(memalloc main.c)

target_link_libraries(memalloc utils)
target_link_libraries(memalloc core)

