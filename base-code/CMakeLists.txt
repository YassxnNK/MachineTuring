cmake_minimum_required(VERSION 3.10.0)
project(project-elec VERSION 1.0)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_subdirectory(src)

