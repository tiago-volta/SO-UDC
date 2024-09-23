#ifndef FUNCTIONS_H
#define FUNCTIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <fcntl.h>     // Para open()
#include <stdbool.h>
#include <sys/types.h> // Para definir tipos de datos como mode_t
#include <sys/stat.h>  // Para definir permisos de archivo
#include <unistd.h>  // Para close()
#include "HistoryList.h"
#include "CMDlist.h"
#include "CommandList.h"

#define LENGTH_MAX 1024
#define MAXNAME 256
#define MAXFILES 100

void printPrompt();

void readInput(bool *finished, CommandList *commands, HistoryList *history);

void processInput(const char *str, char *pieces[], int numPieces, CommandList *commands, HistoryList *history);
int SplitString(const char *str, char *pieces[]);


void AddToCommandList(char *command[],HistoryList *lista);

void AddToHistoryList(char *command,HistoryList *lista);

void command_authors(char * str[]) ;
void command_pid(char * tr[]);
void command_ppid(char * tr[]);
void command_cd(char * tr[]);
void command_date(char * tr[]);
void command_historic (char trozos[LENGTH_MAX]);
void command_open(char * tr[]);
void command_close(char * tr[]);
void command_dup(char * tr[]);
void command_infosys(char * tr[]);
void command_help(char * tr[]);
void command_exit(char * tr[]);



#endif
