//Manejo de la lista de ficheros abiertos

#include "CMDlist.h"
#include <stdio.h>
#include <string.h>

// Función para inicializar la lista de ficheros abiertos
void InitializeOpenFileList(OpenFileList *list) {
    list->numOpenFiles = 0;
    // Opcionalmente, podemos limpiar los nombres y otros campos para evitar valores basura
    for (int i = 0; i < MAXFILES; i++) {
        list->files[i].df = -1;  // Indicador de que el descriptor está vacío
        list->files[i].mode = 0;
        memset(list->files[i].name, 0, sizeof(list->files[i].name));  // Limpiar nombre
    }
}

// Función para listar los ficheros abiertos
void ListOpenFiles(OpenFileList *list) {
    if (list->numOpenFiles == 0) {
        printf("No hay ficheros abiertos\n");
        return;
    }
    for (int i = 0; i < list->numOpenFiles; i++) {
        printf("Descriptor: %d, Name: %s, Mode: %d\n", list->files[i].df, list->files[i].name, list->files[i].mode);
    }
}

// Función para añadir un fichero a la lista
void AddToOpenFiles(OpenFileList *list, int df, int mode, char *name) {
    if (list->numOpenFiles >= MAXFILES) {
        printf("No se pueden abrir más ficheros\n");
        return;
    }
    list->files[list->numOpenFiles].df = df;
    list->files[list->numOpenFiles].mode = mode;
    strcpy(list->files[list->numOpenFiles].name, name);
    list->numOpenFiles++;
}

// Función para eliminar un fichero de la lista
void RemoveFromOpenFiles(OpenFileList *list, int df) {
    for (int i = 0; i < list->numOpenFiles; i++) {
        if (list->files[i].df == df) {
            for (int j = i; j < list->numOpenFiles - 1; j++) {
                list->files[j] = list->files[j + 1];
            }
            list->numOpenFiles--;
            return;
        }
    }
}

// Función para obtener el nombre de un fichero por su descriptor
char* GetFileNameByDescriptor(OpenFileList *list, int df) {
    for (int i = 0; i < list->numOpenFiles; i++) {
        if (list->files[i].df == df) {
            return list->files[i].name;
        }
    }
    return NULL;
}


//Añadir ficheros abiertos al historial va a devolver un int que sera el numero actual de ficheros abiertos que lleva
