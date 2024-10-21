/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 14 / 10 / 24
 */

#include "FileList.h"

#include "HistoryList.h"

// Función auxiliar para crear un nuevo nodo
bool createNodeF(tPosF *p) {
    *p = malloc(sizeof(struct tNode));
    return *p != FNULL; // Devuelve true si la asignación es exitosa
}

void createEmptyListF(OpenFileList *L){                                 //Se pasa por referencia porque se quiere modificar
    *L = FNULL;                                                 //Hacemos que L apunte a un NULL
}

bool isEmptyList(OpenFileList L){
    return L == FNULL;                                          //Miramos si L apunta a  NULL
}

tPosF firstF(OpenFileList L){
    return L;                                                   //El primer elemento de la lista es el nodo al que apunta L
}

tPosF lastF(OpenFileList L){
    tPosF p;
    if (L == FNULL) {  // Si la lista está vacía, no hay último nodo
        return FNULL;
    }
    for(p = L; p->next != FNULL; p = p->next);                  //p recorre la lista hasta que encuentre un nodo cuyo campo next es NULL, es decir es decir el campo next de la última posición, el cual consta como NULL por no apuntar a otro nodo.
    return p;
}

tPosF nextF(tPosF p, OpenFileList L){                                   //Devuelve el nodo al que apunta el campo next de la variable introducida
    return p->next;
}

bool insertItemF(tItemF d, OpenFileList *L) {                   //Se pasa por referencia porque se quiere modificar
    tPosF q, p;
    if (!createNodeF(&q)) {
        return false;
    }
    q->data = d;
    q->next = FNULL;
    if (*L == FNULL) {                                          //Inserción en lista vacía
        *L = q;
    } else {                                                      //Inserción en cualquier sitio
        p = lastF(*L);
        p->next = q;
    }
    return true;
}

tPosF findFile(const int df,OpenFileList L) {
    tPosF p;
    if (!isEmptyList(L)) {
        p = FNULL;
    } else {
        for (p = L; p != NULL && p->data.df != df; p = p->next);
    }
    return p;
}

tItemF defineItem(const int df, const int mode, const char *name) {
    tItemF p;
    p.df = df;
    p.mode = mode;
    strncpy(p.name, name,MAXNAME - 1);      //-1 para dejar espacio al barra 0
    p.name[MAXNAME - 1] = '\0';
    return p;
}


// Función para obtener el nombre de un fichero por su descriptor
tItemF getItemF(tPosF p, OpenFileList L){
    return p->data;                                    //Devuelve el campo data de la posicion p
}

// Función para eliminar un fichero de la lista
void deleteOpenFileAtPositionF(tPosF p,OpenFileList *L){
    tPosF q;
    if(p == *L)						                     //Caso 1: borrado al principio
        *L = (*L)->next;
    else if(p->next == FNULL){                           //Caso 2: borrado al final
        for(q = *L; q->next != p; q = q->next);
        q->next = FNULL;
    }else{                                               //Caso 3: borrado en el medio
        q = p->next;
        p->data = q->data;
        p->next = q->next;
        p = q;
    }
    free(p);
}

void CleanListF(OpenFileList *L) {
    tPosF p;
    while (*L != FNULL) {
        p = *L;
        *L = (*L)->next;
        free(p);
    }
}

// Función para listar los ficheros abiertos
void printListF(OpenFileList L) {
    tPosF p = L;
    if (p == FNULL) {
        printf("No hay ficheros abiertos\n");
    } else {
        while (p != FNULL) {
            printf("Descriptor: %d, Name: %s, Mode: %d\n", p->data.df, p->data.name, p->data.mode);
            p = p->next;
        }
    }
}

