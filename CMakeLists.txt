cmake_minimum_required(VERSION 3.29)
project(P0_CLion_ C)

set(CMAKE_C_STANDARD 99)

add_executable(P0_CLion_ main.c
        HistoryList.h
        HistoryList.c
        functions.h
        functions.c
        CommandList.h
        CommandList.c
        FileList.c
        FileList.h

)
