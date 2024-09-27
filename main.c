/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */

#include "functions.h"

int main(void){
    bool finished = false;
    CommandList cmdList;
    HistoryList historyList;
    OpenFileList openFileList;
    PredefinedCommands(&cmdList);
    createEmptyList(&historyList);
    InitializeOpenFileList(&openFileList);
    while (!finished) {
        printPrompt();
        readInput(&finished,&cmdList,&historyList,&openFileList);
    }
    return 0;
}
