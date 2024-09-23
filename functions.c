#include "functions.h"

//Cada función es un comando que se puede ejecutar en el shell

//Muestra el prompt para que el usuario sepa que puede ingresar un comando
void printPrompt(){
    printf("→ ");
}

//Divide la cadena de entrada en "piezas" utilizando espacios, saltos de línea y tabulaciones como delimitadores
int SplitString(char *str, char *pieces[]) {
    int i = 1;
    if ((pieces[0] = strtok(str, " \n\t")) == NULL)
        return 0;
    while ((pieces[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

//Procesa la entrada del usuario, comparando el comando con la lista de comandos disponibles y ejecutando la función correspondiente
void processInput(const char *str, const char *pieces[], int numPieces, CommandList *commandList, HistoryList *history) {
    for (int i = 0; i < commandList->total; i++) {
        if (strcmp(commandList->commands[i].name, pieces[0]) == 0) {
            AddToHistoryList(str, history);  // Añade el comando al historial
            commandList->commands[i].func(pieces);  // Ejecuta el comando correspondiente
            return;
        }
    }
    perror("Comando inválido, escriba \"help\" para ver los comandos disponibles");
}

//Función que lee la entrada del usuario desde el terminal y la procesa para ejecutar el comando introducido
void readInput(bool *finished, CommandList *commands, HistoryList *history) {
    char input[LENGTH_MAX];
    if (fgets(input, sizeof(input), stdin) != NULL) {
        char *pieces[LENGTH_MAX];
        const char *str = input;
        printf("La cadena es %s\n", input);
        int numPieces = SplitString(input, pieces);
        if (numPieces > 0) {
            printf("Primera pieza es: %s\n", pieces[0]);
            processInput(str, pieces, numPieces, commands, history);  // Procesa la entrada del usuario
        }
    } else {
        perror("Error leyendo la entrada");
    }
}

//Añade un comando introducido a la lista de comandos ejecutados (historial)
void AddToCommandList(char *command[], HistoryList *lista){
    Item newItem;
    insertCommand(newItem, LNULL, lista);  // Inserta el comando en la lista de comandos
}

//Añade un comando introducido al historial
void AddToHistoryList(char *command, HistoryList *lista){
    Item newItem;
    insertCommand(newItem, LNULL, lista);  // Inserta el comando en el historial
}

//Muestra los nombres o los logins de los autores dependiendo de los argumentos proporcionados
void command_authors(const char * str) {
    const char * names[] = {"Pablo Herrero","Tiago Volta"};
    const char * logins[] = {"pablo.herrero.diaz","tiago.velosoevolta"};
    int NumAuthors = 2;
    if (strcmp(&str[1],"-l")==0 || &str[1] == NULL) {
        printf("Logins de los autores: \n");
        for(int i = 0; i < NumAuthors; i++) {
            printf("%s\n", logins[i]);
        }
    } else if(strcmp(&str[1],"-n")==0 || &str[1] == NULL) {
        printf("Nombres de los autores: \n");
        for(int i = 0; i < NumAuthors; i++) {
            printf("%s\n", names[i]);
        }
    }
}

//Muestra el PID del proceso actual
void command_pid(char *str[]) {
    printf("PID: %d\n", getpid());
}

//Muestra el PPID del proceso padre
void command_ppid(char *str[]) {
    printf("PPID: %d\n", getppid());
}

//Comando que cambia el directorio de trabajo
void command_cd(char * tr[]) {
    // Implementación futura
}

//Muestra la fecha y hora actuales
void command_date(char * tr[]) {
    // Implementación futura
}

//Comando que muestra el historial de comandos introducidos
void command_historic (char trozos[LENGTH_MAX]) {
    // Implementación futura
}

//Función que abre un fichero a partir de su descriptor
void command_open(char *pieces[]) {
    int i, df, mode = 0;

    // Si no se especificó un archivo, listar los archivos abiertos
    if (pieces[0] == NULL) {
        ListOpenFiles();  // Lista los archivos abiertos
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
        AddToOpenFiles(df, mode, pieces[0]);  // Añade el archivo a la lista de archivos abiertos
        printf("Añadida entrada a la tabla ficheros abiertos: descriptor %d, archivo %s, modo %d\n", df, pieces[0], mode);
    }
}

//Función que cierra un archivo a partir de su descriptor
void command_close(char *pieces[]) {
    int df;

    // Si no se especifica un descriptor o el descriptor es menor que 0, listar los archivos abiertos
    if (pieces[0] == NULL || (df = atoi(pieces[0])) < 0) {
        ListOpenFiles();  // Lista los archivos abiertos
        return;
    }

    // Intentar cerrar el archivo con el descriptor especificado
    if (close(df) == -1) {
        perror("Imposible cerrar descriptor");
    } else {
        RemoveFromOpenFiles(df);  // Elimina el archivo de la lista de archivos abiertos
    }
}

//Función que duplica un descriptor de archivo
void command_dup(char *pieces[]) {
    int df, duplicated;
    char aux[MAXNAME], *p;

    // Si no hay un descriptor válido, lista los archivos abiertos
    if (pieces[0] == NULL || (df = atoi(pieces[0])) < 0) {
        ListOpenFiles();  // Lista los archivos abiertos
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

//Comando que muestra información del sistema
void command_infosys(char * tr[]) {
    // Implementación futura
}

//Comando que muestra los comandos disponibles
void command_help(char * tr[]) {
    // Implementación futura
}

//Comando para salir del shell
void command_exit(char * tr[]) {
    // Implementación futura
}
