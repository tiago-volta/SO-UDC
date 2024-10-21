/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */

#ifndef FILELIST_H
#define FILELIST_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXNAME 25
#define FNULL NULL


//Estructura para almacenar los ficheros abiertos
typedef struct{
    int df;             //Descriptor de un archivo
    int mode;           //Modo en que se abrió el archivo
    char name[MAXNAME];                     //La lista de canciones de cada usuario está implementada de forma estática
} tItemF;

typedef struct tNode* tPosF;
typedef tPosF OpenFileList;

// Estructura para almacenar un file
struct tNode{
    tItemF data;
    tPosF next;
};

//Declaraciones de funciones
void createEmptyListF(OpenFileList *L);
bool isEmptyList(OpenFileList L);
tPosF firstF(OpenFileList L);
tPosF lastF(OpenFileList L);
tPosF nextF(tPosF p, OpenFileList L);
bool insertItemF(tItemF d, OpenFileList *L);
tPosF findFile(const int df,OpenFileList L);
tItemF defineItem(const int df, const int mode, const char *name);
tItemF getItemF(tPosF p, OpenFileList L);
void deleteOpenFileAtPositionF(tPosF p,OpenFileList *L);
void CleanListF(OpenFileList *L);
void printListF(OpenFileList L);



#endif //FILELIST_H
