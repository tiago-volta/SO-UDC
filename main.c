#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define MAXNAME 256
#define MAXFILES 100

//Estructura para almacenar los ficheros abiertos
typedef struct{
    int df;
    int mode;
    char name[MAXNAME];
} FicheroAbierto;

FicheroAbierto ficherosAbiertos[MAXFILES];
int numFicherosAbiertos = 0;

void ListarFicherosAbiertos () {
    if(numFicherosAbiertos == 0){
        printf("No hay ficheros abiertos\n");
        return;
    }
    for(int i = 0; i < numFicherosAbiertos; i++){
        printf("Descriptor: %d, Nombre: %s, Modo: %d\n", ficherosAbiertos[i].df, ficherosAbiertos[i].name, ficherosAbiertos[i].mode);
    }
}

void AnadirAFicherosAbiertos (int df, int mode, char *name) {
    if(numFicherosAbiertos >= MAXFILES){
        printf("No se pueden abrir mas ficheros\n");
        return;
    }
    ficherosAbiertos[numFicherosAbiertos].df = df;
    ficherosAbiertos[numFicherosAbiertos].mode = mode;
    strcpy(ficherosAbiertos[numFicherosAbiertos].name, name);
    numFicherosAbiertos++;
}


void EliminarDeFicherosAbiertos (int df) {
    for(int i = 0; i < numFicherosAbiertos; i++){
        if(ficherosAbiertos[i].df == df){
            for(int j = i; j < numFicherosAbiertos - 1; j++){
                ficherosAbiertos[j] = ficherosAbiertos[j + 1];
            }
            numFicherosAbiertos--;
            return;
        }
    }
}

char* NombreFicheroDescriptor (int df) {
    for(int i = 0; i < numFicherosAbiertos; i++){
        if(ficherosAbiertos[i].df == df){
            return ficherosAbiertos[i].name;
        }
    }
    return NULL;
}

//Función que abre un fichero a partir de su descriptor
void Cmd_open (char * tr[]) {
    int i, df, mode=0;

    //Si no se especificó un archivo, listar los archivos abiertos
    if (tr[0]==NULL) {
        ListarFicherosAbiertos();
        return;
    }
    //Procesar los parámetros para determinar el modo de apertura del archivo
    for (i=1; tr[i]!=NULL; i++)
        if (!strcmp(tr[i],"cr")) mode|=O_CREAT;
        else if (!strcmp(tr[i],"ex")) mode|=O_EXCL;
        else if (!strcmp(tr[i],"ro")) mode|=O_RDONLY;
        else if (!strcmp(tr[i],"wo")) mode|=O_WRONLY;
        else if (!strcmp(tr[i],"rw")) mode|=O_RDWR;
        else if (!strcmp(tr[i],"ap")) mode|=O_APPEND;
        else if (!strcmp(tr[i],"tr")) mode|=O_TRUNC;
        else break;

    //Intentar abrir el archivo con el modo especificado
    if ((df=open(tr[0],mode,0777))==-1)
        // Si no se puede abrir, mostrar un mensaje de error
            perror ("Imposible abrir fichero");
    else{
        //Si se abrió correctamente, añadir el archivo a la lista de ficheros abiertos
        AnadirAFicherosAbiertos (df,mode,tr[0]);
        printf ("Anadida entrada a la tabla ficheros abiertos: descriptor %d, archivo %s, modo %d\n ",df,tr[0],mode);
    }
}

//Función que cierra un archivo a partir de su descriptor
void Cmd_close(char *tr[]) {
    int df;

    //Si no se especifica un descriptor o el descriptor es menor que 0, listar los archivos abiertos
    if (tr[0] == NULL || (df = atoi(tr[0])) < 0) {
        ListarFicherosAbiertos();
        return;
    }

    //Intentar cerrar el archivo con el descriptor especificado
    if (close(df) == -1)
        //Si ocurre un error al cerrar, se imprime un mensaje de error
            perror("Imposible cerrar descriptor");
    else
        //Si se cierra correctamente, se elimina el archivo de la lista de archivos abiertos
            EliminarDeFicherosAbiertos(df);
}

//Función que duplica un descriptor de archivo
void Cmd_dup(char *tr[]) {
    int df, duplicado;
    char aux[MAXNAME], *p;

    // Si no hay un descriptor válido, lista los archivos abiertos
    if (tr[0] == NULL || (df = atoi(tr[0])) < 0) {
        ListarFicherosAbiertos();  // Llama a la función que lista archivos abiertos
        return;
    }

    // Duplica el descriptor
    if ((duplicado = dup(df)) == -1) {
        perror("Imposible duplicar descriptor");
        return;
    }

    // Obtiene el nombre del archivo asociado con el descriptor original
    p = NombreFicheroDescriptor(df);

    // Prepara una cadena para mostrar la información del duplicado
    snprintf(aux, MAXNAME, "dup %d (%s)", df, p);

    // Añade el nuevo descriptor duplicado a la lista de archivos abiertos
    AnadirAFicherosAbiertos(duplicado, fcntl(duplicado, F_GETFL), aux);
}


int main(void) {
    return 0;
}
