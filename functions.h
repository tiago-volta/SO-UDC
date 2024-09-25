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
#include "CommandList.h"
#include "HistoryList.h"
#include "CMDlist.h"

#define LENGTH_MAX 1024
#define MAXNAME 256
#define MAXFILES 100
#define MAX 4096
#define LNULL (-1)

void printPrompt();
void readInput(bool *finished, CommandList *commandList, HistoryList *history,OpenFileList *openFileList);
void processInput(const char *str,const char *trozos[LENGTH_MAX], int numtrozos, CommandList *commandList, HistoryList *history, OpenFileList *openFileList);
int SplitString(char *str, char *trozos[]);

void AddToCommandList(char *command[],HistoryList *lista);

void AddToHistoryList(const char *command,HistoryList *lista);

void command_authors(char * str) ;
void command_pid();
void command_ppid();
void command_cd(char * str);
void command_date(char * str);
void command_historic (const char *str,HistoryList *lista);
void command_open(char *trozos[],OpenFileList *openFileList);
void command_close(char *trozos[], OpenFileList *openFileList);
void command_dup(char *trozos[], OpenFileList *openFileList);
void command_infosys();
void command_help(char * trozos[],CommandList *commandList);
void command_exit(CommandList *commandList, HistoryList *history, OpenFileList *openFileList);




#endif
