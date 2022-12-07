cmake_minimum_required(VERSION 3.4.1)

LINK_LIBRARIES(m) 
add_library(core SHARED memory_helper.c fit_methods.c)

