//
// Created by pablojhd on 16/09/24.
//

#ifndef FUNCTIONS_H
#define FUNCTIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <fcntl.h>     // Para open()
#include <stdbool.h>
#include <sys/types.h> // Para definir tipos de datos como mode_t
#include <sys/stat.h>  // Para definir permisos de archivo
#include <sys/utsname.h> // Para obtener información del sistema
#include <unistd.h>  // Para close()

#include "HistoryList.h"
#include "CMDlist.h"
#include "CommandList.h"


void printPrompt();
void readInput(bool *finished, CommandList *commandList, HistoryList *history,OpenFileList *openFileList);
void processInput(bool *finished,const char *str,const char *pieces, int numPieces, CommandList *commandList, HistoryList *history, OpenFileList *openFileList);
int SplitString(char *str, char *pieces[]);
void AddToCommandList(char *command[],HistoryList *lista);
void AddToHistoryList(Item *command,HistoryList *lista);
void PredefinedCommands(CommandList *commandList);

void command_authors(char * pieces[],void*,void*,void*);
void command_pid(void*,void*,void*,void*);
void command_ppid(void*,void*,void*,void*);
void command_cd(char *pieces[],void*,void*,void*);
void command_date(char *pieces[],void*,void*,void*);
void command_historic (char *pieces,HistoryList *list,void*,void*);
void command_open(char *pieces[],OpenFileList *openFileList,void*,void*);
void command_close(char *pieces[],OpenFileList *openFileList,void*,void*);
void command_dup(char *pieces[], OpenFileList *openFileList,void*,void*);
void command_infosys(void*,void*,void*,void*);
void command_help(char * pieces[],CommandList *commandList,void*,void*);
void command_exit(bool *finished,OpenFileList *openFileList, HistoryList *history, CommandList *commandList);

#endif //FUNCTIONS_H
