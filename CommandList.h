#ifndef COMMANDLIST_H
#define COMMANDLIST_H

#include "functions.h"


typedef struct {
    char name[LENGTH_MAX];
    char description[LENGTH_MAX];
    void (*func)(char*[],void *);
}Command;

typedef struct {
    Command commands[MAX];
    int total;
}CommandList;

void initializeCommandList(CommandList *list);
void printCommandList(CommandList list);
void freeDynamicMemory(void *dynamicMemoryPointer);

#endif //COMMANDLIST_H
