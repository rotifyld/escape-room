cmake_minimum_required(VERSION 3.10)
project(escape_room C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-g -O3 -Wall -Wextra")

# http://stackoverflow.com/questions/10555706/
macro(add_executable _name)
    # invoke built-in add_executable
    _add_executable(${ARGV})
    if (TARGET ${_name})
        target_link_libraries(${_name} err)
    endif ()
endmacro()


add_library(err err.c)

add_executable(manager manager.c)
target_link_libraries(manager rt pthread)

add_executable(player player.c)
target_link_libraries(player rt pthread)