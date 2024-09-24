//Código fuente con las funciones que se pueden ejecutar en el shell

#include "functions.h"

//Cada función es un comando que se puede ejecutar en el shell

//Muestra el prompt para que el usuario sepa que puede ingresar un comando
void printPrompt(){
    printf("→ ");
}

//Divide la cadena de entrada en "piezas" utilizando espacios, saltos de línea y tabulaciones como delimitadores
int SplitString(char *str, char *trozos[]) {
    int i = 1;
    if ((trozos[0] = strtok(str, " \n\t")) == NULL)
        return 0;
    while ((trozos[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

//Procesa la entrada del usuario, comparando el comando con la lista de comandos disponibles y ejecutando la función correspondiente
void processInput(const char *str, char *trozos[], int numtrozos, CommandList *commandList, HistoryList *history) {
    //Si el comando es "pid", llama directamente a la función command_pid
    if (strcmp(trozos[0], "pid") == 0) {
        command_pid(trozos);
    }
    //Si el comando es "ppid", llama directamente a la función command_ppid
    else if (strcmp(trozos[0], "ppid") == 0) {
        command_ppid(trozos);
    }
    else {
        //Si no es "pid" ni "ppid", sigue con el procesamiento de la lista de comandos
        for (int i = 0; i < commandList->total; i++) {
            if (strcmp(commandList->commands[i].name, trozos[0]) == 0) {
                AddToHistoryList(str, history);  // Añade el comando al historial
                commandList->commands[i].func(trozos);  // Ejecuta el comando correspondiente
                return;
            }
        }
        perror("Comando inválido, escriba \"help\" para ver los comandos disponibles");
    }
}


//Función que lee la entrada del usuario desde el terminal y la procesa para ejecutar el comando introducido
void readInput(bool *finished, CommandList *commands, HistoryList *history) {
    char input[LENGTH_MAX];
    if (fgets(input, sizeof(input), stdin) != NULL) {
        char *trozos[LENGTH_MAX];
        const char *str = input;
        printf("La cadena es %s\n", input);
        int numtrozos = SplitString(input, trozos);
        if (numtrozos > 0) {
            printf("Primera pieza es: %s\n", trozos[0]);
            processInput(str, trozos, numtrozos, commands, history);  // Procesa la entrada del usuario
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
    const char *names[] = {"Pablo Herrero","Tiago Volta"};
    const char *logins[] = {"pablo.herrero.diaz","tiago.velosoevolta"};
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
void command_cd(char *trozos[]) {
    if (trozos[1] == NULL) {
        // Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("rror obteniendo el directorio actual con getcwd()");
        }
    } else {
        // Cambia el directorio de trabajo actual al directorio especificado
        if (chdir(trozos[1]) != 0) {
            perror("Error cambiando el directorio con chdir()");
        }
    }
}

//Muestra la fecha y hora actuales
void command_date(char *tr[]) {
    time_t t;
    struct tm *tm_info;
    char buffer[26];

    //Obtiene la hora actual
    time(&t);
    tm_info = localtime(&t);

    //Formatea la fecha y hora en una cadena
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    //Imprime la fecha y hora actuales
    printf("Fecha y hora actuales: %s\n", buffer);
}

//Comando que muestra el historial de comandos introducidos
void command_historic(char *trozos[], HistoryList *history) {
    //Verifica si se especificó un número de comandos a mostrar
    int n = (trozos[1] != NULL) ? atoi(trozos[1]) : -1;

    //Si no se especificó un número válido, muestra el historial completo
    if (n <= 0) {
        printList(*history);
    } else {
        printLastN(*history, n);
    }
}

//Función que abre un fichero a partir de su descriptor
void command_open(char *trozos[]) {
    int i, df, mode = 0;

    // Si no se especificó un archivo, listar los archivos abiertos
    if (trozos[0] == NULL) {
        ListOpenFiles();  // Lista los archivos abiertos
        return;
    }

    // Procesar los parámetros para determinar el modo de apertura del archivo
    for (i = 1; trozos[i] != NULL; i++) {
        if (!strcmp(trozos[i], "cr")) mode |= O_CREAT;
        else if (!strcmp(trozos[i], "ex")) mode |= O_EXCL;
        else if (!strcmp(trozos[i], "ro")) mode |= O_RDONLY;
        else if (!strcmp(trozos[i], "wo")) mode |= O_WRONLY;
        else if (!strcmp(trozos[i], "rw")) mode |= O_RDWR;
        else if (!strcmp(trozos[i], "ap")) mode |= O_APPEND;
        else if (!strcmp(trozos[i], "tr")) mode |= O_TRUNC;
        else break;
    }

    // Intentar abrir el archivo con el modo especificado
    if ((df = open(trozos[0], mode, 0777)) == -1) {
        perror("Imposible abrir fichero");
    } else {
        AddToOpenFiles(df, mode, trozos[0]);  // Añade el archivo a la lista de archivos abiertos
        printf("Añadida entrada a la tabla ficheros abiertos: descriptor %d, archivo %s, modo %d\n", df, trozos[0], mode);
    }
}

//Función que cierra un archivo a partir de su descriptor
void command_close(char *trozos[]) {
    int df;

    // Si no se especifica un descriptor o el descriptor es menor que 0, listar los archivos abiertos
    if (trozos[0] == NULL || (df = atoi(trozos[0])) < 0) {
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
void command_dup(char *trozos[]) {
    int df, duplicated;
    char aux[MAXNAME], *p;

    // Si no hay un descriptor válido, lista los archivos abiertos
    if (trozos[0] == NULL || (df = atoi(trozos[0])) < 0) {
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
