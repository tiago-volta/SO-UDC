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

bool isEmptyListC(CommandList list) {
    return list.total == CNULL;
}

void createEmptyListC(CommandList *list) {        //Se pasa la lista por referencia porque se quiere modificar su contenido
    list->total = CNULL;                 //Asignar el valor nulo a la variable lastPos para inicializar la lista vacía
}

void printCommandListC(CommandList list) {
    printf("Comandos disponibles:\n");
    for (int i = 0; i < list.total; i++) {
        printf("%s: %s\n",list.commands[i]->name,list.commands[i]->description);
    }
}

void CleanCommandListC(CommandList *list) {
    list->total = CNULL;
}

bool insertCommandC(CommandList *list, const char name[LENGTH_MAX_NAME], const char description[LENGTH_MAX_DESCRIPTION], const int ID) {
    if (list->total == LENGTH_MAX_LIST - 1)                      //Si la última tPosHición de la lista es la última tPosHición del array, no se puede insertar el elemento
        return false;
    else {
        list->total++;
        list->commands[list->total] = malloc(sizeof(Command));
        if (list->commands[list->total] == NULL) {
            return false;
        }
        strncpy(list->commands[list->total]->name,name,LENGTH_MAX_NAME - 1);
        strncpy(list->commands[list->total]->description,description,LENGTH_MAX_DESCRIPTION - 1);
        list->commands[list->total]->ID = ID;
        return true;
    }
}

int FindCommandC(CommandList *list, const char name[LENGTH_MAX_NAME]) {
    if (isEmptyListC(*list))                                 //Si la lista está vacía devuelve nulo
        return CNULL;
    else {
        for (int i=0; i<list->total ;i++) {
            if(strcmp(list->commands[i]->name,&name[0]) == 0){
                return i;
            }
        }                                               //Si son iguales devuelve la posición en la que se encuentra la cancion
        return CNULL;                                   //Si no son iguales, es decir la canción no existe en la lista, devuelve nulo
    }
}