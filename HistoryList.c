/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 14 / 10 / 24
 */

#include "HistoryList.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//Esta funcion crea una lista vacía y ademas la inicializa
void createEmptyListH(HistoryList *L) {        //Se pasa la lista por referencia porque se quiere modificar su contenido
    L->lastPos = HNULL;                 //Asignar el valor nulo a la variable lastPos para inicializar la lista vacía
}

//Recupera el contenido de un elemento cualquiera de la lista
tItemH *getItemH(tPosH p, HistoryList *L){
    return L->data[p];                           //Devuelve el elemento dada la tPosHición el la lista
}

tPosH lastH(HistoryList L){
    return L.lastPos;                                   //Devuelve la última tPosHición con contenido de la lista
}

//Inserta un elemento en la lista antes de la tPosHición indicada, si la tPosHicion es LNULL se añade al final
bool insertCommandH(tItemH *d, HistoryList *L) {      //Se pasa la lista por referencia porque se quiere modificar
    if (L->lastPos == MAX - 1)                      //Si la última tPosHición de la lista es la última tPosHición del array, no se puede insertar el elemento
        return false;
    else {
        L->lastPos++;
        L->data[L->lastPos] = malloc(sizeof(tItemH));
        if (L->data[L->lastPos] == NULL) {
            return false;
        }
        strcpy(*L->data[L->lastPos], *d);
        return true;
    }
}


void printListH(HistoryList *L) {
    if (L->lastPos == -1) {
        printf("La lista de historial está vacía.\n");
        return;
    }
    printf("History List:\n");
    for (int i = 0; i <= L->lastPos; i++) {
        printf("%d: %s\n", i, *L->data[i]);
    }
}


void printLastNH (HistoryList *L, const int n) {
    if (L->lastPos >= n-1) {
        printf("Last %d comands List: \n",n);
        for (int i = L->lastPos - (n-1) ; i<= L->lastPos; i++) {
            printf("%d: %s\n", i,*L->data[i]);
        }
    }else
        perror("No se han ejecutado comandos suficientes");
}

//Funcion para limpiar el historial
void CleanListH(HistoryList *L) {
    for (int i = 0; i <= L->lastPos; i++) {
        free(L->data[i]);
    }
    L->lastPos = HNULL;
}