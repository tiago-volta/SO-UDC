/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */

#ifndef COMMANDLIST_H
#define COMMANDLIST_H

#include <stdio.h>

#define LENGTH_MAX_NAME 15
#define LENGTH_MAX_DESCRIPTION 150

#define LENGTH_MAX_LIST 25
#define CNULL (-1)

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
void CleanCommandList(CommandList *list);



#endif //COMMANDLIST_H
