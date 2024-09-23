#include "HistoryList.h"

#include <stdio.h>


//Esta funcion crea una lista vacía y ademas la inicializa
void createEmptyList(HistoryList *L) {        //Se pasa la lista por referencia porque se quiere modificar su contenido
    L->lastPos = LNULL;                 //Asignar el valor nulo a la variable lastPos para inicializar la lista vacía
}

//Comprueba si la lista esta vacia
bool isEmptyList(HistoryList L){
    return L.lastPos ==LNULL;           //Si la última posición es LNULL la lista está vacía y entonces devuelve true
}

//Devuelve la posicion del primer elemento de la lista
Pos first(HistoryList L){
    return 0;                           //Siempre retorna 0 porque al ser un lista la primera posición siempre es el 0
}

//Recupera el contenido de un elemento cualquiera de la lista
Item *getItem(Pos p, HistoryList L){
    return L.data[p];                           //Devuelve el elemento dada la posición el la lista
}

//Inserta un elemento en la lista antes de la posición indicada, si la posicion es LNULL se añade al final
bool insertCommand(Item *d, Pos p, HistoryList *L) {      //Se pasa la lista por referencia porque se quiere modificar
    Pos i;
    if (L->lastPos == MAX - 1)                      //Si la última posición de la lista es la última posición del array, no se puede insertar el elemento
        return (false);
    else {
        L->lastPos++;
        if (p == LNULL)                             //Si la posición dada es nula, se inserta al final de la lista
            strcpy(L->data[L->lastPos], d);
        else {
            for (i = L->lastPos; i > p; i--){       //Inserta el elemento en la posición dada y las posiciones de los elementos posteriores varían.
                strcpy(L->data[i], L->data[i - 1]);
            }
            strcpy(L->data[p], d);
        }
        return (true);
    }
}

//Funciones del comando

void printList(HistoryList L) {
    printf("Comands List: \n");
    for (int i = 0; i <= L.lastPos; i++) {
        printf("%d: %s\n", i,L.data[i]);
    }
}

void repeatCommand(Pos p, HistoryList L) {
    char *trozos[LENGTH_MAX];
    Item *comando = getItem(p, L);
    int NumTrozos =TrocearCadena(*comando,trozos);
    procesarEntrada(trozos, NumTrozos);
}

void printLastN (HistoryList L, int n) {
    printf("Last %d comands List: \n",n);
    if (L.lastPos > n-2) {
        for (int i = L.lastPos; i >= 0 && n != 0; i--,n--) {
            printf("%d: %s\n", i,L.data[i]);
        }
    }else
        perror("No se han ejecutado comandos suficientes");
}