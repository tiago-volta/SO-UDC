//
// Created by pablojhd on 22/09/24.
//

#ifndef COMMANDLIST_H
#define COMMANDLIST_H

#include <stdio.h>

#define LENGTH_MAX 1024
#define MAX 4096

typedef struct {
    char name[LENGTH_MAX];
    char description[LENGTH_MAX];
    void (*func)(void *, void *, void *, void *);
    int numArgs;
}Command;

typedef struct {
    Command commands[MAX];
    int total;
}CommandList;

void printCommandList(CommandList list);


#endif //COMMANDLIST_H
