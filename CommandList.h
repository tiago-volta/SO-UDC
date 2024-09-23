#ifndef COMMANDLIST_H
#define COMMANDLIST_H
#include "functions.h"

typedef struct {
    char name[1024];
    void (*func)(char*[]);
}Command;

typedef struct {
    Command commands[MAX];
    int total;
}CommandList;

void initializeCommandList(CommandList *list);





#endif //COMMANDLIST_H
