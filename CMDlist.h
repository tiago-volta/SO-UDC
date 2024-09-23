#include <fcntl.h>

#ifndef CMDLIST_H
#define CMDLIST_H
#include "functions.h"


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
