//Manejo de la lista de ficheros abiertos

#include "CMDlist.h"
#include <stdio.h>
#include <string.h>

OpenFile openFiles[MAXFILES];
int numOpenFiles = 0;

void ListOpenFiles() {
    if (numOpenFiles == 0) {
        printf("No hay ficheros abiertos\n");
        return;
    }
    for (int i = 0; i < numOpenFiles; i++) {
        printf("Descriptor: %d, Name: %s, Mode: %d\n", openFiles[i].df, openFiles[i].name, openFiles[i].mode);
    }
}

void AddToOpenFiles(int df, int mode, char *name) {
    if (numOpenFiles >= MAXFILES) {
        printf("No se pueden abrir más ficheros\n");
        return;
    }
    openFiles[numOpenFiles].df = df;
    openFiles[numOpenFiles].mode = mode;
    strcpy(openFiles[numOpenFiles].name, name);
    numOpenFiles++;
}

void RemoveFromOpenFiles(int df) {
    for (int i = 0; i < numOpenFiles; i++) {
        if (openFiles[i].df == df) {
            for (int j = i; j < numOpenFiles - 1; j++) {
                openFiles[j] = openFiles[j + 1];
            }
            numOpenFiles--;
            return;
        }
    }
}

char* GetFileNameByDescriptor(int df) {
    for (int i = 0; i < numOpenFiles; i++) {
        if (openFiles[i].df == df) {
            return openFiles[i].name;
        }
    }
    return NULL;
}


//Añadir ficheros abiertos al historial va a devolver un int que sera el numero actual de ficheros abiertos que lleva
