//
// Created by pablojhd on 22/09/24.
//

#ifndef COMMANDLIST_H
#define COMMANDLIST_H
#include "functions.h"

typedef struct {
    char name[LENGTH_MAX];
    char description[LENGTH_MAX];
    void *func;
    int numArgs;
}Command;

typedef struct {
    Command commands[MAX];
    int total;
}CommandList;

void initializeCommandList(CommandList *list);
void printCommandList(CommandList list);
void clearHistoryList(HistoryList *L);



#endif //COMMANDLIST_H
