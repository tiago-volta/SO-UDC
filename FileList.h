/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Práctica 1
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 25 / 10 / 24
 */

#ifndef FILELIST_H
#define FILELIST_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXNAME 25  //Tamaño maximo del nombre de un fichero
#define FNULL NULL  //Definicion de un puntero a NULL


//Estructura para almacenar información sobre los ficheros abiertos
typedef struct {
    int df;               //Descriptor de archivo
    int mode;             //Modo en que se abrió el archivo
    char name[MAXNAME];   //Nombre del archivo
} tItemF;

typedef struct tNode* tPosF;  //Puntero a la estructura tNode
typedef tPosF OpenFileList;   //OpenFileList es un puntero a la estructura tNode

//Estructura para representar un nodo en la lista enlazada de archivos abiertos
struct tNode {
    tItemF data;         //Datos del archivo almacenados en el nodo
    tPosF next;          //Puntero al siguiente nodo en la lista enlazada
};

//Declaraciones de funciones
void createEmptyListF(OpenFileList *L);
bool isEmptyList(OpenFileList L);
tPosF firstF(OpenFileList L);
tPosF lastF(OpenFileList L);
tPosF nextF(tPosF p);
bool insertItemF(tItemF d, OpenFileList *L);
tPosF findFile(const int df,OpenFileList L);
tItemF defineItem(const int df, const int mode, const char *name);
tItemF getItemF(tPosF p);
void deleteOpenFileAtPositionF(tPosF p,OpenFileList *L);
void CleanListF(OpenFileList *L);
void printListF(OpenFileList L);



#endif //FILELIST_H
