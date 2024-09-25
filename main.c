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


int main(void) {
    bool finished = false; //Inicializar la variable que controlará el bucle principal
    HistoryList History;
    CommandList ListaComandos;
    OpenFileList OpenFileList;

    //Inicialización de las estructuras de datos
    createEmptyList(&History);              //Inicializa la lista de historial de comandos
    initializeCommandList(&ListaComandos);  //Inicializa la lista de comandos disponibles
    InitializeOpenFileList(&OpenFileList);  //Inicializa la lista de archivos abiertos

    //Bucle principal del shell
    while (!finished) {
        printPrompt();  //Muestra el prompt al usuario
        readInput(&finished, &ListaComandos, &History, &OpenFileList);  //Lee y procesa la entrada del usuario
    }

    return 0;
}
