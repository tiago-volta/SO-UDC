#include <stdio.h>

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
