//
// Created by pablojhd on 22/09/24.
//

#ifndef COMMANDLIST_H
#define COMMANDLIST_H

#include <stdio.h>

#define LENGTH_MAX_NAME 15
#define LENGTH_MAX_DESCRIPTION 100

#define LENGTH_MAX_LIST 4096

typedef struct {
    char name[LENGTH_MAX_NAME];
    char description[LENGTH_MAX_DESCRIPTION];
    int ID;
}Command;

typedef struct {
    Command commands[LENGTH_MAX_LIST];
    int total;
}CommandList;

void printCommandList(CommandList list);


#endif //COMMANDLIST_H
