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
void processInput(const char *str,const char *trozos[LENGTH_MAX], int numtrozos, CommandList *commandList, HistoryList *history, OpenFileList *openFileList){
    for (int i=0; i<commandList->total ;i++) {
        if(strcmp(commandList->commands[i].name,trozos[0]) == 0){
            AddToHistoryList(str,history);                  //Se guarda en el historial
            if (commandList->commands[i].numArgs == 0) {
                commandList->commands[i].func();
            }else if (commandList->commands[i].numArgs == 1) {
                commandList->commands[i].func(trozos);
            }else if (commandList->commands[i].numArgs == 2) {
                if (strcmp(trozos[0], "historic") == 0 ) {
                    commandList->commands[i].func(trozos, history);
                } else if (strcmp(trozos[0], "help") == 0 ) {
                    commandList->commands[i].func(trozos, commandList);
                }else {
                    commandList->commands[i].func(trozos, openFileList);
                }
            }else {
                commandList->commands[i].func(commandList,history,openFileList);
            }
        }else
            perror("Comando no válido, introduce ""help"" para ver los disponibles");
    }
}



//Función que lee la entrada del usuario desde el terminal y la procesa para ejecutar el comando introducido
void readInput(bool *finished, CommandList *commandList, HistoryList *history, OpenFileList *openFileList) {
    char input[LENGTH_MAX];  //Buffer para almacenar la entrada del usuario
    char *trozos[LENGTH_MAX];  //Array para almacenar las piezas de la cadena de entrada, limitado a 128 tokens

    //Lee la entrada del usuario desde stdin
    if (fgets(input, sizeof(input), stdin) != NULL) {

        //Eliminar el salto de línea al final si lo hay
        input[strcspn(input, "\n")] = 0;

        //Verificar si la cadena es vacía
        if (strlen(input) == 0) {
            printf("Comando vacio, por favor introduzca un comando.\n");
            return;  //Salir de la función si no hay comando
        }

        //Divide la cadena de entrada en piezas
        int NumTrozos = SplitString(input, trozos);

        //Si se obtuvieron piezas de la cadena de entrada
        if (NumTrozos > 0) {
            printf("Primer trozo es: %s\n", trozos[0]);

            //Si el comando es "exit", marcar como terminado el bucle principal ***************
            if (strcmp(trozos[0], "exit") == 0) {
                *finished = true;
                return;
            }

            //Procesa el comando introducido
            processInput(input, trozos, NumTrozos, commandList, history, openFileList);
        } else {
            printf("No se encontraron comandos validos en la entrada.\n");
        }
    } else {
        perror("Error al leer la entrada");
    }
}

//Añade un comando introducido a la lista de comandos ejecutados (historial)
void AddToCommandList(char *command[], HistoryList *lista){
    Item newItem;
    insertCommand(newItem, LNULL, lista);  //Inserta el comando en la lista de comandos
}

//Añade un comando introducido al historial
void AddToHistoryList(char *command, HistoryList *lista){
    Item newItem;
    insertCommand(newItem, LNULL, lista);  //Inserta el comando en el historial
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
        //Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("rror obteniendo el directorio actual con getcwd()");
        }
    } else {
        //Cambia el directorio de trabajo actual al directorio especificado
        if (chdir(trozos[1]) != 0) {
            perror("Error cambiando el directorio con chdir()");
        }
    }
}

//Muestra la fecha y hora actuales
void command_date(char *str[]) {
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
void command_historic (const char *str,HistoryList *lista) {
    char *NoValidCharacter;     //Puntero para detectar errores en la conversión de strtol
    if (&str[1] != NULL) {      //Si el comando es algo más que historic
        const long int n = strtol(&str[1],&NoValidCharacter,10);  //Variable para almacenar el n si lo hay
        int number = (int) n;
        if(NoValidCharacter == '\0') {
            if(number>=0) {
                repeatCommand(number,*lista);
            }else {
                number = -number;       //Cambiar el signo
                printLastN(*lista,number);
            }
        }else if(NoValidCharacter == &str[1]) {
            perror("No se introdujo ningún número válido.\n");
        }else{
            printf("Parte de la cadena no es válida: %s\n", NoValidCharacter);
            printf("Parte numérica leída: %d\n", number);
            if(number>=0) {
                repeatCommand(number,*lista);
            }else {
                number = -number;       //Cambiar el signo
                printLastN(*lista,number);
            }
        }
    }else {
        printList(*lista);
    }
}

//Función que abre un fichero a partir de su descriptor
void command_open(char *trozos[],OpenFileList *openFileList) {
    int i, df, mode = 0;

    // Si no se especificó un archivo, listar los archivos abiertos
    if (trozos[1] == NULL) {
        ListOpenFiles(openFileList);
        return;
    }

    //Procesar los parámetros para determinar el modo de apertura del archivo
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

    //Intentar abrir el archivo con el modo especificado
    if ((df = open(trozos[1], mode, 0777)) == -1) {
        perror("Imposible abrir fichero");
    } else {
        AddToOpenFiles(openFileList,df, mode, trozos[1]);
        printf("Añadida entrada a la tabla ficheros abiertos: descriptor %d, archivo %s, modo %d\n", df, trozos[1], mode);
    }
}

//Función que cierra un archivo a partir de su descriptor
void command_close(char *trozos[], OpenFileList *openFileList) {
    int df;
    char *NoValidCharacter;  //Puntero para detectar errores en la conversión de strtol

    //Si no se especifica un descriptor, o la entrada es inválida, listar los archivos abiertos
    if (trozos[0] == NULL) {
        ListOpenFiles(openFileList);
        return;
    }

    //Convertir el descriptor de archivo de string a entero de forma segura con strtol
    df = strtol(trozos[0], &NoValidCharacter, 10);  //Base 10 para un número decimal

    //Verificar si la conversión fue exitosa y si el valor del descriptor es válido
    if (*NoValidCharacter != '\0' || df < 0) {
        fprintf(stderr, "Error: descriptor de archivo inválido\n");
        return;
    }

    //Intentar cerrar el archivo con el descriptor especificado
    if (close(df) == -1) {
        perror("Imposible cerrar descriptor");
    } else {
        RemoveFromOpenFiles(openFileList, df);
        printf("Descriptor %d cerrado correctamente.\n", df);
    }
}


//Función que duplica un descriptor de archivo
void command_dup(char *trozos[], OpenFileList *openFileList) {
    int df, duplicated;
    char aux[MAXNAME], *p;
    char *NoValidCharacter;  //Para verificar si hay caracteres no válidos en la conversión

    //Si no hay un descriptor válido, lista los archivos abiertos
    if (trozos[0] == NULL) {
        ListOpenFiles(openFileList);
        return;
    }

    //Convertir la cadena a número usando strtol y verificar si es válida
    df = strtol(trozos[0], &NoValidCharacter, 10);

    //Si el valor no es válido o si se introdujo un valor no numérico
    if (*NoValidCharacter != '\0' || df < 0) {
        ListOpenFiles(openFileList);
        return;
    }

    //Duplica el descriptor
    if ((duplicated = dup(df)) == -1) {
        perror("Imposible duplicar descriptor");
        return;
    }

    //Obtiene el nombre del archivo asociado con el descriptor original
    p = GetFileNameByDescriptor(openFileList, df);

    //Prepara una cadena para mostrar la información del duplicado
    snprintf(aux, MAXNAME, "dup %d (%s)", df, p);

    //Añade el nuevo descriptor duplicado a la lista de archivos abiertos
    AddToOpenFiles(openFileList, duplicated, fcntl(duplicated, F_GETFL), aux);
}


//Comando que muestra información del sistema
void command_infosys(char *tr[]) {
    // Obtiene información del sistema
    struct utsname sysinfo;

    // Llama a la función uname para obtener la información del sistema
    if (uname(&sysinfo) == -1) {
        perror("Error al obtener la información del sistema");
        return;
    }

    // Imprime la información del sistema
    printf("Información del sistema:\n");
    printf("Nombre del sistema operativo: %s\n", sysinfo.sysname);
    printf("Nombre del nodo: %s\n", sysinfo.nodename);
    printf("Versión del sistema operativo: %s\n", sysinfo.release);
    printf("Versión del kernel: %s\n", sysinfo.version);
    printf("Arquitectura del hardware: %s\n", sysinfo.machine);
}

//Comando que muestra los comandos disponibles
void command_help(char * trozos[],CommandList *commandList) {
    if (trozos[1] != NULL) {
        for (int i=0; i<commandList->total ;i++) {
            if(strcmp(commandList->commands[i].name,trozos[1]) == 0) {
                printf("%s: %s\n", commandList->commands[i].name,commandList->commands[i].description);
            }else {
                perror("Comando no válido, introduce ""help"" para ver los disponibles");
            }
        }
    }else{
        printCommandList(*commandList);
    }
}

//Comando para salir del shell
void command_exit(char *tr[], OpenFileList *openFileList, HistoryList *history, CommandList *commandList) {
    // Cierra todos los archivos abiertos
    for (int i = 0; i < openFileList->numOpenFiles; i++) {
        if (close(openFileList->files[i].df) == -1) {
            perror("Error al cerrar el archivo");
        }
    }

    //Limpia la lista de archivos abiertos
    openFileList->numOpenFiles = 0;

    //Limpia la lista de historial usando la función del TAD
    clearHistoryList(history);

    //Limpia la lista de comandos disponibles
    commandList->total = 0;

    //Registra el evento de salida
    FILE *logFile = fopen("exit_log.txt", "a");
    if (logFile != NULL) {
        fprintf(logFile, "Shell exited successfully.\n");
        fclose(logFile);
    } else {
        perror("Error al abrir el archivo de registro");
    }

    //Establece una bandera para indicar que el shell debe terminar
    extern bool finished;
    finished = true;

    //Imprime un mensaje de despedida
    printf("Saliendo del shell...\n");
}
