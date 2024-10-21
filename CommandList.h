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
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>

//Implementar como estática de punteros
#define LENGTH_MAX_NAME 15
#define LENGTH_MAX_DESCRIPTION 250

#define LENGTH_MAX_LIST 23  //Lista de 23 punteros a structs
#define CNULL (-1)

typedef int tPosC;

typedef struct {
    char name[LENGTH_MAX_NAME];
    char description[LENGTH_MAX_DESCRIPTION];
    int ID;
}tCommandC;

typedef struct {
    tCommandC *commands[LENGTH_MAX_LIST];
    tPosC lastPos;
}CommandListC;

void createEmptyListC(CommandListC *list);
void printCommandListC(CommandListC list);
void printCommandDescriptionListC(CommandListC list);
void CleanCommandListC(CommandListC *list);
bool insertCommandC(CommandListC *list, const char name[LENGTH_MAX_NAME], const char description[LENGTH_MAX_DESCRIPTION], const int ID);
tPosC FindCommandC(CommandListC *list, const char name[LENGTH_MAX_NAME]);
tCommandC getCommandC(tPosC p, CommandListC L);



#endif //COMMANDLIST_H
