/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */

#include "HistoryList.h"

#include <string.h>
#include <stdio.h>


//Esta funcion crea una lista vacía y ademas la inicializa
void createEmptyList(HistoryList *L) {        //Se pasa la lista por referencia porque se quiere modificar su contenido
    L->lastPos = HNULL;                 //Asignar el valor nulo a la variable lastPos para inicializar la lista vacía
}

//Recupera el contenido de un elemento cualquiera de la lista
Item *getItem(Pos p, HistoryList *L){
    return &L->data[p];                           //Devuelve el elemento dada la posición el la lista
}

//Inserta un elemento en la lista antes de la posición indicada, si la posicion es LNULL se añade al final
bool insertCommand(Item *d, Pos p, HistoryList *L) {      //Se pasa la lista por referencia porque se quiere modificar
    Pos i;
    if (L->lastPos == MAX - 1)                      //Si la última posición de la lista es la última posición del array, no se puede insertar el elemento
        return (false);
    else {
        L->lastPos++;
        if (p == HNULL)                             //Si la posición dada es nula, se inserta al final de la lista
            strcpy(L->data[L->lastPos], *d);
        else {
            for (i = L->lastPos; i > p; i--){       //Inserta el elemento en la posición dada y las posiciones de los elementos posteriores varían.
                strcpy(L->data[i], L->data[i - 1]);
            }
            strcpy(L->data[p], (const char*)d);
        }
        return (true);
    }
}


void printList(HistoryList *L) {
    if (L->lastPos == -1) {
        printf("La lista de historial está vacía.\n");
        return;
    }
    printf("\nHistory List:\n");
    for (int i = 0; i <= L->lastPos; i++) {
        printf("%d: %s\n", i, L->data[i]);
    }
}


void printLastN (HistoryList *L, int n) {
    if (L->lastPos > n-2) {
        printf("Last %d comands List: \n",n);
        for (int i = L->lastPos - 1 ; i >= 0 && n != 0; i--,n--) {
            printf("%d: %s\n", i,L->data[i]);
        }
    }else
        perror("No se han ejecutado comandos suficientes");
}

//Funcion para limpiar el historial
void clearHistoryList(HistoryList *L) {
    L->lastPos = HNULL;
}