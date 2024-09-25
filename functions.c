//
// Created by pablojhd on 16/09/24.
//

#include "functions.h"
#include "CMDlist.h"

#include <signal.h>

/*las funciones entre puntos hay que implementarlas */

//Cada funcion es un comando

void printPrompt(){
    printf("→ ");
}

int SplitString(char *str, char *pieces[]){
    int i=1;
    if ((pieces[0]=strtok(str," \n\t"))==NULL)
        return 0;
    while ((pieces[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

void readInput(bool *finished, CommandList *commandList, HistoryList *history,OpenFileList *openFileList){
    char input[LENGTH_MAX];     //Simplemente sirve para leer la cadena, después de trocearla solo guarda el primer trozo
    if (fgets(input,sizeof(input),stdin) != NULL) {
        char *trozos[LENGTH_MAX];   //Array de strings, es decir, en cada posición se guarda un trozo de la entrada
        const char cadena = input;
        printf("La cadena es %s\n",input);
        int NumTrozos=SplitString(input,trozos);  //Splitea la cadena en trozos
        if (NumTrozos>0) {
            printf("Primer trozo es: %s\n",trozos[0]);
            printf("Segundo trozo es: %s\n",trozos[1]);
            printf("Tercer trozo es: %s\n",trozos[2]);
            printf("Size %lu\n",sizeof(input));
            printf("NumTrozos=%d\n",NumTrozos);
            processInput(&cadena,trozos,NumTrozos,commandList,history,openFileList);
        }
    }else
        perror ("Error al leer la entrada");
}

/*void processInput(const char *str,const char *pieces[LENGTH_MAX], int numPieces, CommandList *commandList, HistoryList *history, OpenFileList *openFileList){
    for (int i=0; i<commandList->total ;i++) {
        if(strcmp(commandList->commands[i].name,pieces[0]) == 0){
            AddToHistoryList(str,history);                  //Se guarda en el historial
            if (strcmp(pieces[0], "historic") == 0 ) {
                commandList->commands[i].func(pieces, history);
            } else if (strcmp(pieces[0], "help") == 0 ) {
                commandList->commands[i].func(pieces, commandList);
            }else if (strcmp(pieces[0], "quit") == 0 ) {
                commandList->commands[i].func(pieces, commandList);
            }else {
                commandList->commands[i].func(pieces, openFileList);
            }
        }else
            perror("Comando no válido, introduce ""help"" para ver los disponibles");
    }
}*/

void processInput(const char *str,const char *pieces[LENGTH_MAX], int numPieces, CommandList *commandList, HistoryList *history, OpenFileList *openFileList){
    for (int i=0; i<commandList->total ;i++) {
        if(strcmp(commandList->commands[i].name,pieces[0]) == 0){
            AddToHistoryList(str,history);                  //Se guarda en el historial
            if (commandList->commands[i].numArgs == 0) {
                commandList->commands[i].func();
            }else if (commandList->commands[i].numArgs == 1) {
                commandList->commands[i].func(pieces);
            }else if (commandList->commands[i].numArgs == 2) {
                if (strcmp(pieces[0], "historic") == 0 ) {
                    commandList->commands[i].func(pieces, history);
                } else if (strcmp(pieces[0], "help") == 0 ) {
                    commandList->commands[i].func(pieces, commandList);
                }else {
                    commandList->commands[i].func(pieces, openFileList);
                }
            }else {
                commandList->commands[i].func(commandList,history,openFileList);
            }
        }else
            perror("Comando no válido, introduce ""help"" para ver los disponibles");
    }
}

void AddToHistoryList(const char *command,HistoryList *lista){
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

//Muestra el PID del proceso actual
void command_pid() {
    printf("PID: %d\n", getpid());
}

//Muestra el PPID del proceso padre
void command_ppid() {
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


/*historic [N|-N] Shows the historic of commands executed by this shell. In order to
do this, a list to store all the commands input to the shell must be
implemented.
– historic Prints all the comands that have been input with their
order number
– historic N Repeats command number N (from historic list)
– historic -N Prints only the lastN comands
Students are free to decide whether historic stars numbering commands
at 0 or at 1. Hypothetically, there’s a scenario where trying to repeat
a historic command could yield an infinite loop or a stack overflow
(depending on how it is coded), so students may choose to not store
calls to historic N itself in the historic list if they want so (See the
NOTES ON LIST IMPLEMENTATIONS at the end of this document)*/

void command_historic (const char *str,HistoryList *list) {
    char *NoValidCharacter;
    if (&str[1] != NULL) {      //Si el comando es algo más que historic
        const long int n = strtol(&str[1],&NoValidCharacter,10);         //Variable para almacenar el n si lo hay
        int number = (int) n;
        if(NoValidCharacter == '\0') {
            if(number>=0) {
                repeatCommand(number,*list);
            }else {
                number = -number;       //Cambiar el signo
                printLastN(*list,number);
            }
        }else if(NoValidCharacter == &str[1]) {
            perror("No se introdujo ningún número válido.\n");
        }else{
            printf("Parte de la cadena no es válida: %s\n", NoValidCharacter);
            printf("Parte numérica leída: %d\n", number);
            if(number>=0) {
                repeatCommand(number,*list);
            }else {
                number = -number;       //Cambiar el signo
                printLastN(*list,number);
            }
        }
    }else {
        printList(*list);
    }


}

void command_open(char *pieces[],OpenFileList *openFileList) {
    int i, df, mode = 0;

    // Si no se especificó un archivo, listar los archivos abiertos
    if (pieces[0] == NULL) {
        ListOpenFiles(openFileList);  // Lista los archivos abiertos
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
        AddToOpenFiles(openFileList,df, mode, pieces[0]);  // Añade el archivo a la lista de archivos abiertos
        printf("Añadida entrada a la tabla ficheros abiertos: descriptor %d, archivo %s, modo %d\n", df, pieces[0], mode);
    }
}
//Función que cierra un archivo a partir de su descriptor
void command_close(char *pieces[],OpenFileList *openFileList) {
    int df;

    // Si no se especifica un descriptor o el descriptor es menor que 0, listar los archivos abiertos
    if (pieces[0] == NULL || (df = atoi(pieces[0])) < 0) {
        ListOpenFiles(openFileList);  // Lista los archivos abiertos
        return;
    }

    // Intentar cerrar el archivo con el descriptor especificado
    if (close(df) == -1) {
        perror("Imposible cerrar descriptor");
    } else {
        RemoveFromOpenFiles(openFileList,df);  // Elimina el archivo de la lista de archivos abiertos
    }
}
void command_dup(char *pieces[], OpenFileList *openFileList) {
    int df, duplicated;
    char aux[MAXNAME], *p;

    // Si no hay un descriptor válido, lista los archivos abiertos
    if (pieces[0] == NULL || (df = atoi(pieces[0])) < 0) {
        ListOpenFiles(openFileList);  // Lista los archivos abiertos
        return;
    }

    // Duplica el descriptor
    if ((duplicated = dup(df)) == -1) {
        perror("Imposible duplicar descriptor");
        return;
    }

    // Obtiene el nombre del archivo asociado con el descriptor original
    p = GetFileNameByDescriptor(openFileList,df);

    // Prepara una cadena para mostrar la información del duplicado
    snprintf(aux, MAXNAME, "dup %d (%s)", df, p);

    // Añade el nuevo descriptor duplicado a la lista de archivos abiertos
    AddToOpenFiles(openFileList,duplicated, fcntl(duplicated, F_GETFL), aux);
}
//Comando que muestra información del sistema
void command_infosys() {
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
void command_help(char * pieces[],CommandList *commandList) {
    if (pieces[1] != NULL) {
        for (int i=0; i<commandList->total ;i++) {
            if(strcmp(commandList->commands[i].name,pieces[1]) == 0) {
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
void command_exit(char *str[], OpenFileList *openFileList, HistoryList *history, CommandList *commandList) {
    // Cierra todos los archivos abiertos
    for (int i = 0; i < openFileList->numOpenFiles; i++) {
        if (close(openFileList->files[i].df) == -1) {
            perror("Error al cerrar el archivo");
        }
    }

    // Limpia la lista de archivos abiertos
    openFileList->numOpenFiles = 0;

    // Limpia la lista de historial usando la función del TAD
    clearHistoryList(history);

    // Limpia la lista de comandos disponibles
    commandList->total = 0;

    // Registra el evento de salida
    FILE *logFile = fopen("exit_log.txt", "a");
    if (logFile != NULL) {
        fprintf(logFile, "Shell exited successfully.\n");
        fclose(logFile);
    } else {
        perror("Error al abrir el archivo de registro");
    }

    // Establece una bandera para indicar que el shell debe terminar
    extern bool finished;
    finished = true;

    // Imprime un mensaje de despedida
    printf("Saliendo del shell...\n");
}