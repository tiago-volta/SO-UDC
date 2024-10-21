/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */

#include "CommandList.h"

#include <stdlib.h>

bool isEmptyListC(CommandListC list) {
    return list.lastPos == CNULL;
}

void createEmptyListC(CommandListC *list) {        //Se pasa la lista por referencia porque se quiere modificar su contenido
    list->lastPos = CNULL;                 //Asignar el valor nulo a la variable lastPos para inicializar la lista vacía
}

void printCommandDescriptionListC(CommandListC list) {
    printf("Comandos disponibles:\n");
    for (int i = 0; i <= list.lastPos; i++) {
        printf("%s %s\n",list.commands[i]->name,list.commands[i]->description);
    }
}

void printCommandListC(CommandListC list) {
    printf("help  [cmd|-all]	Muestra ayuda sobre los comandos \n");
    printf("Comandos disponibles:\n");
    for (int i = 0; i <= list.lastPos; i++) {
        printf("%s ",list.commands[i]->name);
    }
    printf("\n");
}

void CleanCommandListC(CommandListC *list) {
    list->lastPos = CNULL;
}

bool insertCommandC(CommandListC *list, const char name[LENGTH_MAX_NAME], const char description[LENGTH_MAX_DESCRIPTION], const int ID) {
    if (list->lastPos == LENGTH_MAX_LIST - 1)
        return false;
    else {
        list->lastPos++;
        list->commands[list->lastPos] = malloc(sizeof(tCommandC));
        if (list->commands[list->lastPos] == NULL) {
            return false;
        }
        strncpy(list->commands[list->lastPos]->name,name,LENGTH_MAX_NAME - 1);
        strncpy(list->commands[list->lastPos]->description,description,LENGTH_MAX_DESCRIPTION - 1);
        list->commands[list->lastPos]->ID = ID;
        return true;
    }
}

tPosC FindCommandC(CommandListC *list, const char name[LENGTH_MAX_NAME]) {
    if (isEmptyListC(*list))                                 //Si la lista está vacía devuelve nulo
        return CNULL;
    else {
        for (int i=0; i<=list->lastPos ;i++) {
            if(strcmp(list->commands[i]->name,&name[0]) == 0){
                return i;
            }
        }                                               //Si son iguales devuelve la posición en la que se encuentra la cancion
        return CNULL;                                   //Si no son iguales, es decir la canción no existe en la lista, devuelve nulo
    }
}

tCommandC getCommandC(tPosC p, CommandListC list){
    return *list.commands[p];                                   //Devuelve el elemento dada la posición el la lista
}