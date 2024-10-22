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
#define LENGTH_MAX_H 256

typedef int tPosH;
typedef char tItemH[LENGTH_MAX_H];

typedef struct{
    tItemH *data [MAX];
    tPosH lastPos;
}HistoryList;


void createEmptyListH(HistoryList *L);

bool insertCommandH(tItemH *d, HistoryList *L);

tItemH *getItemH(tPosH p, HistoryList *L);

void printListH(HistoryList *L);

void printLastNH (HistoryList *L, const int n);

void CleanListH(HistoryList *L);

tPosH lastH(HistoryList L);


#endif //HISTORYLIST_H
