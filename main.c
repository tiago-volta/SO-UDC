//Inicializa la lista de programas y el historico y ejecuta el bucle principal que lee y procesa los comandos

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "functions.h" // Incluir el archivo de cabecera
#include "HistoryList.h"
#include "CMDlist.h"
#include "CommandList.h"

#define MAXNAME 256
#define MAXFILES 100


int main(void) {
    bool finished = false; // Inicializar la variable
    HistoryList History;
    CommandList ListaComandos;
    OpenFileList OpenFileList;
    createEmptyList(&History);
    initializeCommandList(&ListaComandos);
    InitializeOpenFileList(&OpenFileList);
    while (!finished) {
        printPrompt();
        readInput(&finished,&ListaComandos,&History,&OpenFileList); //Aquí podrías agregar lógica para cambiar `terminado` según sea necesario
    }
    return 0;
}