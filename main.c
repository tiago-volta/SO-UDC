#include "functions.h" // Incluir el archivo de cabecera
#include "HistoryList.h"
#include "CMDlist.h"
#include "CommandList.h"

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
        readInput(&finished,&ListaComandos,&History,&OpenFileList);// Aquí podrías agregar lógica para cambiar `terminado` según sea necesario
    }
    return 0;
}
