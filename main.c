#include "functions.h" // Incluir el archivo de cabecera
#include "HistoryList.h"
#include "CMDlist.h"
#include "CommandList.h"

int main(void) {
    bool terminado = false; // Inicializar la variable
    HistoryList History;
    CommandList ListaComandos;
    createEmptyList(&History);
    initializeCommandList(&ListaComandos);
    while (!terminado) {
        imprimirPromt();
        leerEntrada(&terminado,&ListaComandos,&History);// Aquí podrías agregar lógica para cambiar `terminado` según sea necesario
    }
    return 0;
}
