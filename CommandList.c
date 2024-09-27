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

void printCommandList(CommandList list) {
    printf("Comandos disponibles:\n");
    for (int i = 0; i < list.total; i++) {
        printf("%s: %s\n",list.commands[i].name,list.commands[i].description);
    }
}

void CleanCommandList(CommandList *list) {
    list->total = CNULL;
}