//
// Created by pablojhd on 16/09/24.
//

#include "CMDlist.h"

#include <stdio.h>

/*void ShowOpenFiles(int NumOpenFiles, OpenFile OpenFiles[LENGTH_MAX]) {
    printf("Opening files...\n");
    for (int i = 0; i < NumOpenFiles; i++) {
        printf("Descriptor %d   Nombre %s   Modo %s\n",OpenFiles[i].fd, OpenFiles[i].name, fcntl(OpenFiles[i].fd,F GETFL));
    }
}
*/
//Añadir ficheros abiertos al historial va a devolver un int que sera el numero actual de ficheros abiertos que lleva
