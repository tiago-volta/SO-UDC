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

//Función auxiliar para crear un nuevo nodo
bool createNodeF(tPosF *p) {
    *p = malloc(sizeof(struct tNode));
    return *p != FNULL; // Devuelve true si la asignación es exitosa
}

//Inicializa la lista de archivos abiertos como vacía
void createEmptyListF(OpenFileList *L){                                 //Se pasa por referencia porque se quiere modificar
    *L = FNULL;                                                 //Hacemos que L apunte a un NULL
}

//Verifica si la lista de archivos abiertos está vacía
bool isEmptyList(OpenFileList L){
    return L == FNULL;                                          //Miramos si L apunta a  NULL
}

//Retorna la primera posición de la lista
tPosF firstF(OpenFileList L){
    return L;                                                   //El primer elemento de la lista es el nodo al que apunta L
}

//Retorna el último nodo de la lista
tPosF lastF(OpenFileList L){
    tPosF p;
    if (L == FNULL) {  //Si la lista está vacía, no hay último nodo
        return FNULL;
    }
    for(p = L; p->next != FNULL; p = p->next);                  //p recorre la lista hasta que encuentre un nodo cuyo campo next es NULL, es decir es decir el campo next de la última posición, el cual consta como NULL por no apuntar a otro nodo.
    return p;
}

//Retorna el nodo siguiente al nodo proporcionado
tPosF nextF(tPosF p){                                   //Devuelve el nodo al que apunta el campo next de la variable introducida
    return p->next;
}

//Inserta un nuevo archivo en la lista de archivos abiertos
bool insertItemF(tItemF d, OpenFileList *L) {                          //Se pasa por referencia porque se quiere modificar
    tPosF q, p;
    if (!createNodeF(&q)) {   //Crea un nuevo nodo
        return false;   //Retorna false si no se pudo asignar memoria
    }
    q->data = d;   //Asigna los datos al nuevo nodo
    q->next = FNULL;
    if (*L == FNULL) {   //Caso 1: inserción en lista vacía
        *L = q;   //El nuevo nodo es el primer nodo
    } else {   //Caso 2: inserción en lista no vacía
        p = lastF(*L);   //Busca el último nodo
        p->next = q;   //Añade el nuevo nodo al final
    }
    return true;
}

//Busca un archivo en la lista por su descriptor
tPosF findFile(const int df, OpenFileList L) {
    tPosF p;
    if (isEmptyList(L)) {   //Si la lista está vacía
        p = FNULL;   //Retorna NULL
    } else {
        for (p = L; p != NULL && p->data.df != df; p = p->next);   //Recorre la lista hasta encontrar el archivo
    }
    return p;   //Retorna la posición o NULL si no se encuentra
}

//Define un nuevo archivo con su descriptor, modo y nombre
tItemF defineItem(const int df, const int mode, const char *name) {
    tItemF p;
    p.df = df;   //Asigna el descriptor de archivo
    p.mode = mode;   //Asigna el modo del archivo
    strncpy(p.name, name, MAXNAME - 1);   //Copia el nombre del archivo, asegurándose de dejar espacio para el terminador null
    p.name[MAXNAME - 1] = '\0';   //Añade el terminador null al final del nombre
    return p;
}


//Función para obtener el nombre de un fichero por su descriptor
tItemF getItemF(tPosF p){
    return p->data;                                    //Devuelve el campo data de la posicion p
}

//Función para eliminar un fichero de la lista
void deleteOpenFileAtPositionF(tPosF p, OpenFileList *L) {
    tPosF q;
    if (p == *L) {   //Caso 1: eliminación en el primer nodo
        *L = (*L)->next;
    } else if (p->next == FNULL) {   //Caso 2: eliminación en el último nodo
        for (q = *L; q->next != p; q = q->next);   //Busca el nodo anterior al último
        q->next = FNULL;    //Elimina el enlace al nodo a eliminar
    } else {   //Caso 3: eliminación en el medio
        q = p->next;
        p->data = q->data;   //Copia los datos del siguiente nodo al actual
        p->next = q->next;   //Ajusta los enlaces
        p = q;   //El nodo a eliminar es el siguiente
    }
    free(p);   //Libera la memoria del nodo eliminado
}

//Limpia completamente la lista de archivos abiertos, liberando toda la memoria
void CleanListF(OpenFileList *L) {
    tPosF p;
    while (*L != FNULL) {   //Mientras la lista no esté vacía
        p = *L;            //Guarda la posición actual
        *L = (*L)->next;   //Avanza al siguiente nodo
        free(p);          //Libera la memoria del nodo actual
    }
}

//Función para listar los ficheros abiertos
void printListF(OpenFileList L) {
    tPosF p = L;
    if (p == FNULL) { //Si la lista está vacía
        printf("No hay ficheros abiertos\n");
    } else {
        while (p != FNULL) {
            printf("Descriptor: %d, Name: %s, Mode: %d\n", p->data.df, p->data.name, p->data.mode); //Imprime los datos del archivo
            p = p->next; //Avanza al siguiente nodo
        }
    }
}

