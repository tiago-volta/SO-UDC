/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */

#include "CommandList.h"

#include <stdlib.h>

//Verifica si la lista de comandos está vacía
bool isEmptyListC(CommandListC list) {
    return list.lastPos == CNULL;
}

//Crea una lista vacía
void createEmptyListC(CommandListC *list) {        //Se pasa la lista por referencia porque se quiere modificar su contenido
    list->lastPos = CNULL;                 //Asignar el valor nulo a la variable lastPos para inicializar la lista vacía
}

//Imprime los nombres y descripciones de todos los comandos en la lista
void printCommandDescriptionListC(CommandListC list) {
    printf("Comandos disponibles:\n");
    for (int i = 0; i <= list.lastPos; i++) {
        printf("%s %s\n", list.commands[i]->name, list.commands[i]->description);  //Imprime el nombre y la descripción de cada comando
    }
}

//Imprime solo los nombres de los comandos disponibles en la lista
void printCommandListC(CommandListC list) {
    printf("help  [cmd|-all]    Muestra ayuda sobre los comandos \n");  //Imprime el comando 'help'
    printf("Comandos disponibles:\n");
    for (int i = 0; i <= list.lastPos; i++) {
        printf("%s ", list.commands[i]->name);  //Imprime el nombre de cada comando
    }
    printf("\n");
}

//Limpia la lista de comandos liberando la memoria ocupada y reinicializando la lista
void CleanCommandListC(CommandListC *list) {
    freeCommandList(list);  //Libera a memória antes de limpar
    list->lastPos = CNULL;  //Reinicializa la lista
}

//Inserta un nuevo comando en la lista si no está llena, copiando su nombre, descripción y asignando su ID
bool insertCommandC(CommandListC *list, const char name[LENGTH_MAX_NAME], const char description[LENGTH_MAX_DESCRIPTION], const int ID) {
    //Verifica si la lista ya está llena
    if (list->lastPos == LENGTH_MAX_LIST - 1)
        return false;
    else {
        //Avanza la posición e intenta asignar memoria para el nuevo comando
        list->lastPos++;
        list->commands[list->lastPos] = malloc(sizeof(tCommandC));
        if (list->commands[list->lastPos] == NULL) {
            return false;  //Retorna false si la asignación falla
        }

        //Copia el nombre y garantiza la terminación null
        strncpy(list->commands[list->lastPos]->name, name, LENGTH_MAX_NAME - 1);
        list->commands[list->lastPos]->name[LENGTH_MAX_NAME - 1] = '\0';  //Garantiza que sea null-terminated

        //Copia la descripción y también garantiza la terminación null
        strncpy(list->commands[list->lastPos]->description, description, LENGTH_MAX_DESCRIPTION - 1);
        list->commands[list->lastPos]->description[LENGTH_MAX_DESCRIPTION - 1] = '\0';  //Garantiza que acabe en null

        //Asigna el ID al comando
        list->commands[list->lastPos]->ID = ID;

        return true;  //Retorna true si la inserción fue exitosa
    }
}

//Busca un comando por su nombre en la lista y devuelve su posición, o CNULL si no lo encuentra
tPosC FindCommandC(CommandListC *list, const char name[LENGTH_MAX_NAME]) {
    if (isEmptyListC(*list))                                 //Si la lista está vacía devuelve nulo
        return CNULL;
    else {
        for (int i=0; i<=list->lastPos ;i++) {
            if(strcmp(list->commands[i]->name,&name[0]) == 0){
                return i;
            }
        }                                               //Si son iguales devuelve la posición en la que se encuentra la cancion
        return CNULL;                                   //Si no son iguales, es decir la canción no existe en la lista, devuelve nulo
    }
}

//Retorna el comando en la posición dada en la lista
tCommandC getCommandC(tPosC p, CommandListC list){
    return *list.commands[p];
}

//Libera la memoria ocupada por todos los comandos en la lista
void freeCommandList(CommandListC *list) {
    for (int i = 0; i <= list->lastPos; i++) {
        if (list->commands[i] != NULL) {
            free(list->commands[i]);  //Libera cada comando alocado con el malloc
        }
    }
}
