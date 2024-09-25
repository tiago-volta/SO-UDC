//
// Created by pablojhd on 22/09/24.
//

#include "CommandList.h"


void printCommandList(CommandList list) {
    printf("Comandos disponibles:\n");
    for (int i = 0; i < list.total; i++) {
        printf("%s: %s\n",list.commands[i].name,list.commands[i].description);
    }
}
