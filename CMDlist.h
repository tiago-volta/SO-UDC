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

// Estructura para gestionar la lista de ficheros abiertos
typedef struct {
    OpenFile files[MAXFILES];
    int numOpenFiles;
} OpenFileList;

//Declaraciones de funciones
void InitializeOpenFileList(OpenFileList *list);
char* GetFileNameByDescriptor(OpenFileList *list, int df);
void ListOpenFiles(OpenFileList *list);
void AddToOpenFiles(OpenFileList *list, int df, int mode, char *name);
void RemoveFromOpenFiles(OpenFileList *list, int df);




#endif //CMDLIST_H