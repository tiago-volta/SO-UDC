#include "functions.h" // Incluir el archivo de cabecera


int main() {
    bool finished = false; // Inicializar la variable
    HistoryList History;
    CommandList ListaComandos;
    OpenFileList OpenFileList;
    createEmptyList(&History);
    PredefinedCommands(&ListaComandos);
    InitializeOpenFileList(&OpenFileList);
    while (!finished) {
        printPrompt();
        readInput(&finished,&ListaComandos,&History,&OpenFileList);// Aquí podrías agregar lógica para cambiar `terminado` según sea necesario
    }
    return 0;
}
