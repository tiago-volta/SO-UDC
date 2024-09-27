//
// Created by pablojhd on 26/09/24.
//

#include "functions.h"

void printPrompt(){
    printf("→ ");
}

//Funcion auxiliar para el read no tiene que estar en la cabecera
int SplitString(char *str, char *pieces[]){
    int i=1;
    if ((pieces[0]=strtok(str," \n\t"))==NULL)
        return 0;
    while ((pieces[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}


void readInput(bool *finished, CommandList *commandList, HistoryList *history,OpenFileList *openFileList){
    char input[LENGTH_MAX_INPUT];     //Simplemente sirve para leer la cadena, después de trocearla solo guarda el primer trozo
    if (fgets(input,sizeof(input),stdin) != NULL) {
        char *trozos[LENGTH_MAX_INPUT];   //Array de strings, es decir, en cada posición se guarda un trozo de la entrada
        Item cadena;
        strcpy(cadena,input);       //Guardo una copia de la cadena en el historial
        int NumTrozos=SplitString(input,trozos);  //Splitea la cadena en trozos
        if (NumTrozos>0) {
            processInput(finished,cadena,trozos,commandList,history,openFileList);
        }
    }else
        perror ("Error al leer la entrada");
}


void PredefinedCommands(CommandList *commandList) {
    // Lista de comandos y descripciones
    const char *names[] = {
        "authors", "pid", "ppid", "cd", "date",
        "historic", "open", "close", "dup", "infosys",
        "help", "quit", "exit", "bye"
    };

    const char *descriptions[] = {
        "Prints the names and logins of the program authors. authors -l prints only the logins. authors -n prints only the names.",
        "Prints the pid of the process executing the shell.",
        "Prints the pid of the shell’s parent process.",
        "Changes the current working directory of the shell to dir. Without arguments it prints the current working directory.",
        "Prints the current date and the current time. date -d Prints only the current date. date -t Prints only the current time.",
        "Shows the history of commands executed by this shell. historic N repeats command number N. historic -N prints only the last N commands.",
        "open without arguments lists the shell open files. open file, opens a file and adds it to the list of shell open files.",
        "Closes the df file descriptor and eliminates the corresponding item from the list.",
        "Duplicates the df file descriptor creating the corresponding new entry on the file list.",
        "Prints information on the machine running the shell.",
        "help displays a list of available commands. help cmd gives a brief help on the usage of command cmd.",
        "Ends the shell.",
        "Ends the shell.",
        "Ends the shell."
    };

    // Inicializar el número total de comandos
    commandList->total = sizeof(names) / sizeof(names[0]);

    // Copiar los valores en la estructura CommandList
    for (int i = 0; i < commandList->total; i++) {
        strncpy(commandList->commands[i].name, names[i], LENGTH_MAX_NAME - 1);
        strncpy(commandList->commands[i].description, descriptions[i], LENGTH_MAX_DESCRIPTION - 1);
        commandList->commands[i].ID = i;  // Asignar el ID único
    }
}

void AddToHistoryList(Item *command,HistoryList *lista){
    Item *newItem = command;
    insertCommand(newItem,LNULL,lista);
}

int getCmdIdAndHistory (Item *str,const char *pieces[],CommandList *commandList,HistoryList *history) {
    for (int i=0; i<commandList->total ;i++) {
        if(strcmp(commandList->commands[i].name,pieces[0]) == 0){
            AddToHistoryList(str,history);                  //Se guarda en el historial
            return commandList->commands[i].ID;
        }
    }
    return -1;
}

void processInput(bool *finished,Item *str,char *pieces[], CommandList *commandList, HistoryList *history,OpenFileList *fileList){
    switch (getCmdIdAndHistory(str,pieces,commandList,history)) {
        case 0:
            command_authors(pieces);
            break;
        case 1:
            command_pid();
            break;
        case 2:
            command_ppid();
            break;
        case 3:
            command_cd(pieces);
            break;
        case 4:
            command_date(pieces);
            break;
        case 5:
            command_historic(pieces,finished,commandList,history,fileList);
            break;
        case 6:
            command_open(pieces,fileList);
            break;
        case 7:
            command_close(pieces,fileList);
            break;
        case 8:
            command_dup(pieces,fileList);
            break;
        case 9:
            command_infosys();
            break;
        case 10:
            command_help(pieces,commandList);
            break;
        case 11:
        case 12:
        case 13:
            command_exit(finished,fileList,history,commandList);
            break;
        default:
            perror("Comando no válido, introduce \"help\" para ver los disponibles");
            break;
    }
}

void command_authors(char * pieces[]) {
    const char * names[] = {"Pablo Herrero","Tiago Volta"};
    const char * logins[] = {"pablo.herrero.diaz","tiago.velosoevolta"};
    int NumAuthors = 2;
    if(pieces[1] == NULL || strcmp(pieces[1],"-n")==0) {
        printf("Nombres de los autores: \n");
        for(int i=0; i<NumAuthors; i++) {
            printf("%s\n",names[i]);
        }
    }
    if (pieces[1] == NULL || strcmp(pieces[1],"-l") == 0) {
        printf("Logins de los autores: \n");
        for(int i=0; i<NumAuthors; i++) {
            printf("%s\n",logins[i]);
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
void command_cd(char *pieces[]) {
    if (pieces[1] == NULL) {
        // Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual con getcwd()");
        }
    } else {
        // Cambia el directorio de trabajo actual al directorio especificado
        if (chdir(pieces[1]) != 0) {
            perror("Error cambiando el directorio con chdir()");
        }
    }
}

//Muestra la fecha y hora actuales, cambiar para que procese las cosas pedidas
void command_date(char *pieces[]) {
    time_t t;
    struct tm *tm_info;
    char buffer[26];

    // Obtiene la hora actual
    time(&t);
    tm_info = localtime(&t);

    if (pieces[1] == NULL || strcmp(pieces[1], "-t") == 0) {
        // Si no hay argumentos o si se especifica "-t", imprime solo la hora
        strftime(buffer, 26, "%H:%M:%S", tm_info);
        printf("Hora actual: %s\n", buffer);
    }

    if (pieces[1] == NULL || strcmp(pieces[1], "-d") == 0) {
        // Si no hay argumentos o si se especifica "-d", imprime solo la fecha
        strftime(buffer, 26, "%Y-%m-%d", tm_info);
        printf("Fecha actual: %s\n", buffer);
    }
}


void repeatCommand(Pos p,bool *finished,char *pieces[], CommandList *commandList, HistoryList *history, OpenFileList *openFileList){
    char *trozos[LENGTH_MAX];
    Item *comando = getItem(p, history);
    SplitString(*comando,trozos);
    processInput(finished,comando,trozos,commandList,history,openFileList);
}

void command_historic (char *pieces[],bool *finished,CommandList *commandList, HistoryList *history, OpenFileList *openFileList) {
    char *NoValidCharacter;
    if (pieces[1] != NULL) {      //Si el comando es algo más que historic
        const long int n = strtol(pieces[1],&NoValidCharacter,10);         //Variable para almacenar el n si lo hay
        int number = (int) n;
            if(*NoValidCharacter == '\0') {
                if(0 <= number <= history->lastPos) {
                    repeatCommand(number,finished,pieces,commandList,history,openFileList);
                }else if (number < 0) {
                    number = -number;       //Cambiar el signo
                    printLastN(history,number);
                }else
                    perror("No se han ejecutado comandos suficientes");
            }else if(NoValidCharacter == pieces[1]) {
                perror("No se introdujo ningún número válido.\n");
            }else{
                printf("Parte de la cadena no es válida: %s\n", NoValidCharacter);
                printf("Parte numérica leída: %d\n", number);
                if(0 <= number <= history->lastPos) {
                    repeatCommand(number,finished,pieces,commandList,history,openFileList);
                }else if (number < 0) {
                    number = -number;       //Cambiar el signo
                    printLastN(history,number);
                }else
                    perror("No se han ejecutado comandos suficientes");
            }

    }else {
        printList(history);
    }
}

void command_open(char *pieces[],OpenFileList *openFileList) {
    int i, df, mode = 0;

    // Si no se especificó un archivo, listar los archivos abiertos
    if (pieces[1] == NULL) {
        ListOpenFiles(openFileList);  // Lista los archivos abiertos
        return;
    }

    // Procesar los parámetros para determinar el modo de apertura del archivo
    for (i = 2; pieces[i] != NULL; i++) {
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
    if ((df = open(pieces[1], mode, 0777)) == -1) {
        perror("Imposible abrir archivo");
    } else {
        AddToOpenFiles(openFileList,df, mode, pieces[1]);  // Añade el archivo a la lista de archivos abiertos
        printf("Archivo abierto: descriptor %d, archivo %s, modo %d\n", df, pieces[1], mode);
    }
}

//Función que cierra un archivo a partir de su descriptor, mejorar si da tiempo
void command_close(char *pieces[],OpenFileList *openFileList) {
    int df;

    // Si no se especifica un descriptor o el descriptor es menor que 0, listar los archivos abiertos
    if (pieces[1] == NULL || (df = atoi(pieces[1])) < 0) {
        ListOpenFiles(openFileList);  // Lista los archivos abiertos
        return;
    }

    // Intentar cerrar el archivo con el descriptor especificado
    if (close(df) == -1) {
        perror("Imposible cerrar archivo");
    } else {
        RemoveFromOpenFiles(openFileList,df);  // Elimina el archivo de la lista de archivos abiertos
        printf("Archivo cerrado: descriptor %d, archivo %s\n", df, pieces[1]);
    }
}

void command_dup(char *pieces[], OpenFileList *openFileList) {
    int df, duplicated;
    char aux[MAXNAME], *p;

    // Si no hay un descriptor válido, lista los archivos abiertos
    if (pieces[1] == NULL || (df = atoi(pieces[1])) < 0) {
        ListOpenFiles(openFileList);  // Lista los archivos abiertos
        return;
    }

    // Duplica el descriptor
    if ((duplicated = dup(df)) == -1) {
        perror("Imposible duplicar descriptor");
        return;
    }

    // Obtiene el nombre del archivo asociado con el descriptor original
    p = GetFileName(openFileList,df);

    // Prepara una cadena para mostrar la información del duplicado
    snprintf(aux, MAXNAME, "dup %d (%s)", df, p);

    // Añade el nuevo descriptor duplicado a la lista de archivos abiertos
    AddToOpenFiles(openFileList,duplicated, fcntl(duplicated, F_GETFL), aux);
    printf("Archivo duplicado: descriptor %d, archivo %s, modo %d\n", df, pieces[1], fcntl(duplicated, F_GETFL));

}

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
                return;
            }
        }
        perror("Comando no válido, introduce ""help"" para ver los disponibles");
    }else{
        printCommandList(*commandList);
    }
}

//Falta por revisar y hacerla modulos
void command_exit(bool *finished,OpenFileList *openFileList, HistoryList *history, CommandList *commandList) {
    // Cierra todos los archives abiertos
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
    *finished = true;

    // Imprime un mensaje de despedida
    printf("Saliendo del shell...\n");
}