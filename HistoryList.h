/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */
#ifndef HISTORYLIST_H
#define HISTORYLIST_H

#include <stdbool.h>
#include <stdio.h>

#define MAX 4096  //Tamaño máximo del array en la implementación estática
#define HNULL (-1) //Constante para indicar la posición nula o lista vacia
#define LENGTH_MAX 100

typedef int Pos;
typedef char Item[LENGTH_MAX];

typedef struct{
    Item data [MAX];
    Pos lastPos;
}HistoryList;


void createEmptyList(HistoryList *L);

bool insertCommand(Item *d, Pos p, HistoryList *L);

Item *getItem(Pos p, HistoryList *L);

void printList(HistoryList *L);

void printLastN (HistoryList *L, int n);

void clearHistoryList(HistoryList *L);


#endif //HISTORYLIST_H
