//
// Created by pablojhd on 22/09/24.
//

#ifndef HISTORYLIST_H
#define HISTORYLIST_H

#include <stdbool.h>
#include <stdio.h>

#define MAX 4096  //Tamaño máximo del array en la implementación estática
#define LNULL (-1) //Constante para indicar la posición nula o lista vacia
#define LENGTH_MAX 1024

typedef int Pos;
typedef char Item[LENGTH_MAX];

typedef struct{
    Item data [MAX];
    Pos lastPos;
}HistoryList;

/*
 * OBJETIVO: crear una lista vacía
 * ENTRADAS: la lista que vamos a crear
 * SALIDAS: una lista vacía (no contiene elementos)
 * POSCONDICIÓN: la lista sin datos queda inicializada
 */
void createEmptyList(HistoryList *L);
/*
 * OBJETIVO: determinar si la lista está vacía
 * ENTRADAS: una lista
 * SALIDAS: verdadero si la lista está vacía, falso si no lo está
 */
bool isEmptyList(HistoryList L);
/*
 * OBJETIVO: conocer la posición del primer elemento de la lista
 * ENTRADAS: una lista
 * SALIDAS: la posición del primer elemento de la lista
 * PRECONDICIÓN: la lista inicializada y no vacía
 */
Pos first(HistoryList L);


/*
 * OBJETIVO: insertar un elemento en la lista antes de la posicion indicada
 * ENTRADAS:
 *  d: el elemento a insertar
 *  p: la posición donde queremos insertar el elemento actual
 *  L: la lista
 * SALIDAS: verdadero si el elemento fue insertado, falso si no se insertó. La lista modificada
 * PRECONDICIÓN: la lista inicializada, no vacía y que la posición introducida sea válida en la lista o nula
 * POSCONDICIÓN: las posiciones de los elementos posteriores a la del elemento insertado pueden haber variado
 */
bool insertCommand(Item *d, Pos p, HistoryList *L);

/*
 * OBJETIVO: obtener un elemento de una cierta posición de la lista
 * ENTRADAS:
 *  p: la posición del elemento buscado
 *  L: la lista
 * SALIDAS: el contenido del elemento que está en esa posición
 * PRECONDICIÓN: la lista inicializada, no vacía y que la posición introducida sea válida en la lista
 */
Item *getItem(Pos p, HistoryList L);
/*
 * OBJETIVO: modificar el contenido de un elemento de la lista
 * ENTRADAS:
 *  d: el nuevo contenido a asignar
 *  p: la posición del elemento a modificar
 *  L: la lista
 * SALIDAS: la lista modificada, con el nuevo elemento
 * PRECONDICIÓN: la lista inicializada, no vacía y que la posición introducida sea válida en la lista
 */

void printList(HistoryList L);

void repeatCommand(Pos p, HistoryList L);

void printLastN (HistoryList L, int n);

void clearHistoryList(HistoryList *L);


#endif //HISTORYLIST_H
