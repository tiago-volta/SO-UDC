cmake_minimum_required(VERSION 3.29)
project(P0_CLion_ C)

set(CMAKE_C_STANDARD 99)

add_executable(P0_CLion_ main.c
        functions.c
        functions.h
        CMDlist.c
        CMDlist.h
        HistoryList.c
        HistoryList.h
        CommandList.c
        CommandList.h
)
