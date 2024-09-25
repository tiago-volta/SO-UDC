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

#define LENGTH_MAX 1024
#define MAXNAME 256
#define MAXFILES 100

void printPrompt();
void readInput(bool *finished, CommandList *commandList, HistoryList *history,OpenFileList *openFileList);
void processInput(const char *str,const char *trozos[LENGTH_MAX], int numtrozos, CommandList *commandList, HistoryList *history, OpenFileList *openFileList);
int SplitString(char *str, char *trozos[]);


void AddToCommandList(char *command[],HistoryList *lista);

void AddToHistoryList(const char *command,HistoryList *lista);

void command_authors(char * str) ;
void command_pid(char * str);
void command_ppid(char * str);
void command_cd(char * str);
void command_date(char * str);
void command_historic (const char *str,HistoryList *lista);
void command_open(char * str);
void command_close(char * str);
void command_dup(char * str);
void command_infosys(char * str);
void command_help(char * str);
void command_exit(char * str);



#endif
