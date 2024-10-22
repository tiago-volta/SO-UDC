//
// Created by pablojhd on 22/10/24.
//

#ifndef COMMANDS_H
#define COMMANDS_H

#include "Functions.h"

#define LENGTH_MAX_INPUT 512
#define LENGTH_MAX_OUTPUT 256
#define LENGTH_MAX_PHRASE 128
#define LENGTH_MAX_PATH 128
#define LENGTH_MAX_FULLPATH 256

void command_authors(char * pieces[]);
void command_pid();
void command_ppid();
void command_cd(char *pieces[]);
void command_date(char *pieces[]);
void command_historic (char *pieces[],bool *finished,CommandListC *commandList, HistoryList *history, OpenFileList *openFileList);
void command_open(char *pieces[],OpenFileList *openFileList);
void command_close(char *pieces[],OpenFileList *openFileList);
void command_dup(char *pieces[], OpenFileList *openFileList);
void command_infosys();
void command_help(char * pieces[], CommandListC *commandList);
void command_exit(bool *finished,OpenFileList *openFileList, HistoryList *history, CommandListC *commandList);

void command_makefile(char *pieces[]);
void command_makedir(char *pieces[]);
void command_listFile(char *pieces[]);
void command_cwd();
void command_listDir(char *pieces[]);
void command_reclist(char *pieces[]);
void command_revlist(char *pieces[]);
void command_delrec (char *pieces[]);
void command_erase(char *pieces[]);


#endif //COMMANDS_H
