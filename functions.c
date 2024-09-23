#include "functions.h"

//Cada funcion es un comando que se puede ejecutar en el shell
void printPrompt(){
    printf("→ ");
}

int SplitString(char *str, char *pieces[]) {
    int i = 1;
    if ((pieces[0] = strtok(str, " \n\t")) == NULL)
        return 0;
    while ((pieces[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

void processInput(const char *str, const char *pieces[], int numPieces, CommandList *commandList, HistoryList *history) {
    for (int i = 0; i < commandList->total; i++) {
        if (strcmp(commandList->commands[i].name, pieces[0]) == 0) {
            AddToHistoryList(str, history);
            commandList->commands[i].func(pieces);
            return;
        }
    }
    perror("Comando inválido, escriba \"help\" para ver los comandos disponibles");
}

//Función que lee la entrada del usuario
void readInput(bool *finished, CommandList *commands, HistoryList *history) {
    char input[LENGTH_MAX];
    if (fgets(input, sizeof(input), stdin) != NULL) {
        char *pieces[LENGTH_MAX];
        const char *str = input;
        printf("La cadena es %s\n", input);
        int numPieces = SplitString(input, pieces);
        if (numPieces > 0) {
            printf("Primera pieza es: %s\n", pieces[0]);
            printf("Segunda pieza es: %s\n", pieces[1]);
            printf("Tercera pieza es: %s\n", pieces[2]);
            printf("Tamaño %lu\n", sizeof(input));
            printf("numPieces = %d\n", numPieces);
            processInput(str, pieces, numPieces, commands, history);
        }
    } else {
        perror("Error reading input");
    }
}

void AddToHistoryList(char *command,HistoryList *lista){
    Item newItem;
    insertCommand(newItem,LNULL,lista);
}

/*authors Prints the names and logins of the program authors. authors -l prints
only the logins and authors -n prints only the names*/
void command_authors(const char * str) {
    const char * names[] = {"Pablo Herrero","Tiago Volta"};
    const char * logins[] = {"pablo.herrero.diaz","tiago.velosoevolta"};
    int NumAuthors = 2;
    if (strcmp(&str[1],"-l")==0 || &str[1] == NULL) {
        printf("Logins de los autores: \n");
        for(int i=0; i<NumAuthors; i++) {
            printf("%s\n",logins[i]);
        }
    }else if(strcmp(&str[1],"-n")==0 || &str[1] == NULL) {
        printf("Nombres de los autores: \n");
        for(int i=0; i<NumAuthors; i++) {
            printf("%s\n",names[i]);
        }
    }
}



void command_historic (char trozos[LENGTH_MAX], int NumTrozos) {
    
}

//Función que abre un fichero a partir de su descriptor
void command_open(char *pieces[]) {
    int i, df, mode = 0;

    // Si no se especificó un archivo, listar los archivos abiertos
    if (pieces[0] == NULL) {
        ListOpenFiles();
        return;
    }

    // Procesar los parámetros para determinar el modo de apertura del archivo
    for (i = 1; pieces[i] != NULL; i++) {
        if (!strcmp(pieces[i], "cr")) mode |= O_CREAT;
        else if (!strcmp(pieces[i], "ex")) mode |= O_EXCL;
        else if (!strcmp(pieces[i], "ro")) mode |= O_RDONLY;
        else if (!strcmp(pieces[i], "wo")) mode |= O_WRONLY;
        else if (!strcmp(pieces[i], "rw")) mode |= O_RDWR;
        else if (!strcmp(pieces[i], "ap")) mode |= O_APPEND;
        else if (!strcmp(pieces[i], "tr")) mode |= O_TRUNC;
        else break;
    }

    // Intentar abrir el archivo con el modo especificado
    if ((df = open(pieces[0], mode, 0777)) == -1) {
        perror("Imposible abrir fichero");
    } else {
        AddToOpenFiles(df, mode, pieces[0]);
        printf("Añadida entrada a la tabla ficheros abiertos: descriptor %d, archivo %s, modo %d\n", df, pieces[0], mode);
    }
}

//Función que cierra un archivo a partir de su descriptor
void command_close(char *pieces[]) {
    int df;

    // Si no se especifica un descriptor o el descriptor es menor que 0, listar los archivos abiertos
    if (pieces[0] == NULL || (df = atoi(pieces[0])) < 0) {
        ListOpenFiles();
        return;
    }

    // Intentar cerrar el archivo con el descriptor especificado
    if (close(df) == -1) {
        perror("Imposible cerrar descriptor");
    } else {
        RemoveFromOpenFiles(df);
    }
}

// Función que duplica un descriptor de archivo
void command_dup(char *pieces[]) {
    int df, duplicated;
    char aux[MAXNAME], *p;

    // Si no hay un descriptor válido, lista los archivos abiertos
    if (pieces[0] == NULL || (df = atoi(pieces[0])) < 0) {
        ListOpenFiles();
        return;
    }

    // Duplica el descriptor
    if ((duplicated = dup(df)) == -1) {
        perror("Imposible duplicar descriptor");
        return;
    }

    // Obtiene el nombre del archivo asociado con el descriptor original
    p = GetFileNameByDescriptor(df);

    // Prepara una cadena para mostrar la información del duplicado
    snprintf(aux, MAXNAME, "dup %d (%s)", df, p);

    // Añade el nuevo descriptor duplicado a la lista de archivos abiertos
    AddToOpenFiles(duplicated, fcntl(duplicated, F_GETFL), aux);
}

