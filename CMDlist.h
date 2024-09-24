#ifndef CMDLIST_H
#define CMDLIST_H

#include "functions.h"
#include <fcntl.h>
#include <string.h>

#define MAXFILES 100
#define MAXNAME 256

//Estructura para almacenar los ficheros abiertos
typedef struct{
    int df;
    int mode;
    char name[MAXNAME];
} OpenFile;

//Variables globales
extern OpenFile OpenFiles[MAXFILES];
extern int numOpenFiles;

//Declaraciones de funciones
char* GetFileNameByDescriptor(int fd);
void ListOpenFiles();
void AddToOpenFiles(int fd, int mode, char *name);
void RemoveFromOpenFiles(int fd);

#endif //CMDLIST_H
