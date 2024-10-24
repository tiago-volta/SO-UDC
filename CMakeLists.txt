
cmake_minimum_required(VERSION 3.29)
project(P0_CLion_ C)

set(CMAKE_C_STANDARD 99)

add_executable(P0_CLion_ main.c
        HistoryList.h
        HistoryList.c
        CommandList.h
        CommandList.c
        FileList.c
        FileList.h
        Functions.h
        Functions.c
        Commands.c
        Commands.h
)