/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */

#include "functions.h"

//Las listas se inicializan todas aqui??

int main(void){
    bool finished = false;
    CommandListC cmdList;
    HistoryList historyList;
    OpenFileList openFileList;
    createEmptyListC(&cmdList);
    InsertPredefinedCommands(&cmdList);
    createEmptyListH(&historyList);
    createEmptyListF(&openFileList);
    while (!finished) {
        printPrompt();
        readInput(&finished,&cmdList,&historyList,&openFileList);
    }
    return 0;
}
