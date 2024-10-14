/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 1
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */

#include "functions.h"

#include <errno.h>


//Imprime el prompt
void printPrompt(){
    printf("→ ");
    fflush(stdout);
}

//Funcion auxiliar para el read
static int SplitString(char *str, char *pieces[]){
    int i=1;
    if ((pieces[0]=strtok(str," \n\t"))==NULL)
        return 0;
    while ((pieces[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}


void readInput(bool *finished, CommandList *commandList, HistoryList *history,OpenFileList *openFileList){
    char input[LENGTH_MAX_INPUT];
    if (fgets(input,LENGTH_MAX_INPUT,stdin) != NULL) {
        char *trozos[LENGTH_MAX_INPUT];
        Item cadena;
        strcpy(cadena,input);       //Guardo una copia de la cadena en el historial
        size_t len = strlen(cadena);
        if (len > 0 && cadena[len - 1] == '\n') {
            cadena[len - 1] = '\0';  // Reemplazo '\n' con '\0' para que luego en el historial no de problemas al imprimir la cadena
        }
        int NumTrozos=SplitString(input,trozos);  //Splitea la cadena en trozos
        if (NumTrozos>0) {
            processInput(finished,&cadena,trozos,commandList,history,openFileList);
        }
    }else
        perror ("Error al leer la entrada");
}

//Puedo hacerla privada?? y como
void PredefinedCommands(CommandList *commandList) {
    const char *names[] = {
        "authors", "pid", "ppid", "cd", "date",
        "historic", "open", "close", "dup", "infosys",
        "makefile","makedir","listfile","cwd","listdir",
        "reclist", "revlist","erase","delrec",
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
        "Creates a new file with the specified name. makefile filename creates a new empty file.",
        "Creates a new directory with the specified name. makedir dirname creates a new directory.",
        "Lists the files in the current directory or the specified directory. listfile lists all files, with options for details.",
        "Prints the current working directory of the shell.",
        "Lists directories contents",
        "Lists directories recursively (subdirectories after).",
        "Lists directories recursively (subdirectories before).",
        "Deletes files and/or empty directories.",
        "Deletes files and/pr non empty directories recursively.",
        "help displays a list of available commands. help cmd gives a brief help on the usage of command cmd.",
        "Ends the shell.",
        "Ends the shell.",
        "Ends the shell."
    };

    // Obtenemos el numero total de comandos dividiendo el tamaño total entre el tamaño de un comando
    commandList->total = sizeof(names) / sizeof(names[0]);

    // Copiamos los valores en el struct CommandList
    for (int i = 0; i < commandList->total; i++) {
        strncpy(commandList->commands[i].name, names[i], LENGTH_MAX_NAME - 1);
        strncpy(commandList->commands[i].description, descriptions[i], LENGTH_MAX_DESCRIPTION - 1);
        commandList->commands[i].ID = i;  // Asignar un ID único de cada comando
    }
}

//Función auxiliar para guardar en el historial
static void AddToHistoryList(Item *command,HistoryList *lista){
    Item *newItem = command;
    insertCommand(newItem,HNULL,lista);
}

//Obtenemos el ID del comando para luego poder elegir en el switch, además aprovechamos y guardamos en el historial
static int getCmdIdAndHistory (Item *str,char *pieces[],CommandList *commandList,HistoryList *history) {
    for (int i=0; i<commandList->total ;i++) {
        if(strcmp(commandList->commands[i].name,pieces[0]) == 0){
            AddToHistoryList(str,history);                  //Se guarda en el historial
            return commandList->commands[i].ID;
        }
    }
    return -1;
}

//Procesa el comando introducido //Se puede hacer privada??
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
            command_makefile(pieces);
            break;
        case 11:
            command_makedir(pieces);
            break;
        case 12:
            command_listFile(pieces);
            break;
        case 13:
            command_cwd();
            break;
        case 14:
            command_listDir(pieces);
            break;
        case 15:
            command_reclist(pieces);
            break;
        case 16:
            command_revlist(pieces);
            break;
        case 17:
            command_erase(pieces);
            break;
        case 18:
            command_delrec(pieces);
            break;
        case 19:
            command_help(pieces,commandList);
            break;
        case 20:
        case 21:
        case 22:
            command_exit(finished,fileList,history,commandList);
            break;
        default:
            perror("Comando no válido, introduce \"help\" para ver los disponibles");
            break;
    }
}

//Imprime el nombre y/o los logins de los autores
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
    if (pieces[1] == NULL) {                            // Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    } else {                                            // Cambia el directorio de trabajo actual al directorio especificado
        if (chdir(pieces[1]) != 0) {
            perror("Error cambiando el directorio");
        }
    }
}

//Muestra la fecha y hora actuales
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

//Funcion auxiliar para repetir un comando guardado en el historial
static void repeatCommand(Pos p,bool *finished, CommandList *commandList, HistoryList *history, OpenFileList *openFileList){
    char *trozos[LENGTH_MAX];
    Item cadena;
    Item *comando = getItem(p, history);
    strcpy(cadena,*comando);
    SplitString(*comando,trozos);
    processInput(finished,&cadena,trozos,commandList,history,openFileList);
}

//Mustra el historial de comandos introducidos, o repite un comando ya introducido o imprimer los últimos n comandos
void command_historic (char *pieces[],bool *finished,CommandList *commandList, HistoryList *history, OpenFileList *openFileList) {
    char *NoValidCharacter;
    if (pieces[1] != NULL) {
        const long int n = strtol(pieces[1],&NoValidCharacter,10);         //Variable para almacenar el n si lo hay
        int number = (int) n;
            if(*NoValidCharacter == '\0') {
                if (0 <= number && number <= history->lastPos) {
                    repeatCommand(number,finished,commandList,history,openFileList);
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
                if (0 <= number && number <= history->lastPos) {
                    repeatCommand(number,finished,commandList,history,openFileList);
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

//Función que abre un archivo con un modo específico
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

//Función que cierra un archivo a partir de su descriptor
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

//Función que duplica un archivo a partir de du identificador
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

//Devuelve información sobre el sistema
void command_infosys() {
    struct utsname sysinfo;

    // Llama a la función uname para obtener la información del sistema
    if (uname(&sysinfo) == -1) {
        perror("Error al obtener la información del sistema");
        return;
    }
    printf("%s (%s), OS: %s - %s - %s \n",sysinfo.nodename,sysinfo.machine,sysinfo.sysname,sysinfo.release,sysinfo.version);
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

//Comando que limpia todas las listas, cierra los archivos y finaliza el programa
void command_exit(bool *finished,OpenFileList *openFileList, HistoryList *history, CommandList *commandList) {
    // Cierra todos los archives abiertos
    for (int i = 0; i < openFileList->numOpenFiles; i++) {
        if (close(openFileList->files[i].df) == -1) {
            perror("Error al cerrar el archivo");
        }
    }
    // Limpiamos las listas utilizadas en el programa
    CleanCommandList(commandList);
    CleanOpenFilesList(openFileList);
    clearHistoryList(history);

    // Establece una bandera para indicar que la shell debe terminar
    *finished = true;

    // Imprime un mensaje de despedida
    printf("Saliendo de la shell...\n");
}

//Sino se le pasa argumento imprime el directorio actual, si se le pasa argumento crea un archivo con ese nomnbre, si ya existe no lo crea
void command_makefile(char *pieces[]) {
    if (pieces[1] == NULL) {                            // Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    }else {
        if (access(pieces[1],F_OK) != -1) {                 //Si el archivo no existe devuelve 0 sino -1
            printf("Error: El archivo '%s' ya existe.\n", pieces[1]);
        }else {
            FILE *logFile = fopen(pieces[1], "w");
            if (logFile != NULL && fclose(logFile) != EOF) {
                printf("Archivo '%s' creado correctamente.\n", pieces[1]);
            }else
                perror("Error al cerrar el archivo");
        }
    }

}

static char LetraTF (mode_t m){
    switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
    }
}

static char *GetPermissions(mode_t m) {
    char *permisos;

    if ((permisos=(char *) malloc (12))==NULL)
        return NULL;
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';     //Permiso de lectura por el propietario
    if (m&S_IWUSR) permisos[2]='w';     //Permiso de escritura por el propietario
    if (m&S_IXUSR) permisos[3]='x';     //Permiso de ejecución por el propietario
    if (m&S_IRGRP) permisos[4]='r';     //Permiso de lectura por un grupo
    if (m&S_IWGRP) permisos[5]='w';     //Permiso de escritura por un grupo
    if (m&S_IXGRP) permisos[6]='x';     //Permiso de ejecución por un grupo
    if (m&S_IROTH) permisos[7]='r';     //Permiso de lectura por otros
    if (m&S_IWOTH) permisos[8]='w';     //Permiso de escritura por otros
    if (m&S_IXOTH) permisos[9]='x';     //Permiso de ejecución por otros
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';

    return permisos;
}

//Crea un directorio
void command_makedir(char *pieces[]) {
    if (pieces[1] == NULL) {
        fprintf(stderr, "No se ha especificado el nombre del directorio\n"); //fprintf con stedrr, porque es un error logico del usuario
        return;
    }
    struct stat st = {0};
    //Verifica si eldirectorio ya existe
    if (stat(pieces[1], &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("Imposible crear '%s', ya existe.\n", pieces[1]);
        return;
    }
    //Crea el directorio con permisos de lectura, escritura y ejecución para el usuario
    if (mkdir(pieces[1], 0777) == -1) { //no tiene permisos para crear el directorio
        perror("Permision denegada");
    } else {
        printf("Directorio creado: %s\n", pieces[1]);

        //Esto creo que no es necesario
        //Obtener información del nuevo directorio
        if (stat(pieces[1], &st) == 0) {  //Volver a llamar a stat para obtener el modo del nuevo directorio
            // Obtener permisos del directorio
            char *permisos = GetPermissions(st.st_mode);  //Convierte el modo a formato legible
            if (permisos != NULL) {
                printf("Permisos del nuevo directorio: %s\n", permisos);
                free(permisos);  //libera la memoria
            } else {
                fprintf(stderr, "Error al asignar memoria para los permisos.\n");
            }
        } else {
            perror("Error al obtener información del nuevo directorio");
        }
    }
}



//gives information on files or directories
/*listfile [-long][-link][-acc] name1 name2 ..	lista ficheros;
-long: listado largo
-acc: acesstime
-link: si es enlace simbolico, el path contenido
*/

void command_listFile(char *pieces[]) {
    if (pieces[2] == NULL) {                            // Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    }else{
        for (int i = 2; pieces[i] != NULL; i++) {
            struct stat data;
            //Poner esta comprobacion dentro haciendo funcion auxiliar porque si el comando - algo es no valido imprimirá dos errores el de esta comprobacion y el de comando no valido
            if (stat(pieces[i],&data) == -1) {
                fprintf(stderr, "Error al intentar acceder a %s: ", pieces[i]);  // Mensaje de error
                perror("");
                continue;       //Sale del bucle aqui y procesa la siguiente iteración
            }
            if (strcmp(pieces[1],"-long") == 0) {
                time_t t = data.st_atim.tv_sec;         //Almacenamos la hora en segundos desde 1970
                struct tm *tm_info = localtime(&t);
                char buffer[26];
                //Formatea la fecha y hora en una cadena
                strftime(buffer, 26, "%Y/%m/%d-%H:%M", tm_info);
                char* permissions = GetPermissions(data.st_mode);       //Permisos
                //Propietario y grupo
                struct passwd *pw = getpwuid(data.st_uid);
                struct group *gr = getgrgid(data.st_gid);
                printf("%s   %ld (%ld) %s %s %s %9ld %s\n",buffer,data.st_nlink,data.st_ino,pw->pw_name,gr->gr_name,permissions,data.st_size,pieces[i]);
                free(permissions);
            }else if (strcmp(pieces[1],"-acc") == 0) {
                printf("%ld %s\n",data.st_size,pieces[i]);
            }else if (strcmp(pieces[1],"-link") == 0) {
                char link[LENGTH_MAX];
                ssize_t linklen = readlink(pieces[i],link,LENGTH_MAX);
                if(linklen == -1) {         //No tiene enlace simbólico
                    printf("%ld %s\n",data.st_size,pieces[i]);
                }else {
                    link[linklen] = '\0';  // Añadimos terminador nulo
                    printf("El enlace simbólico '%s' apunta a '%s'\n", pieces[i], link);
                }
            }else {
                perror("Comando no válido, introduce \"help\" para ver los disponibles");
            }
        }
    }
}

//Printea el directorio actual
void command_cwd() {
    char cwd[LENGTH_MAX]; //buffer para almacenar el directorio actual

    //Obtiene el directorio actual
    if(getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Directorio actual: %s\n", cwd);
    } else {
        perror("Error obteniendo el directorio actual");
    }
}


/*listdir [-hid][-long][-link][-acc] n1 n2 ..	lista contenidos de directorios
    -hid: incluye los ficheros ocultos, es como el acc pero con los hiden
    -acc: acesstime
    -link: si es enlace simbolico, el path contenido
*/

//Funcion auxiliar para obtener los datos de los archivos de un directorio
static bool GetFileData(bool hide,struct stat *data,char *dir,char* name) {
    if (hide && (name[0] == '.' || strcmp(name, "..") == 0)) {
        return false;
    }
    char filePath[LENGTH_MAX_INPUT];
    snprintf(filePath,LENGTH_MAX_INPUT,"%s/%s",dir,name);
    if (stat(filePath,data) == -1) {
        fprintf(stderr, "Error al intentar acceder a %s: ", name);  // Mensaje de error
        perror("");
        return false;
    }
    return true;
}

//Si tiempo hacer un array de options y buscar la opcion con un for para luego elegir con el switch, además comprobar antes si exite pieces[1] que imprimir el directorio actual
void command_listDir(char *pieces[]) {
    if (pieces[2] == NULL) {
        // Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    }else {
        for (int i = 2; pieces[i] != NULL; i++) {
            DIR *dir;
            struct dirent *entry;
            bool hide = true;

            //Abrimos el directorio
            dir = opendir(pieces[i]);
            if (dir == NULL) {
                fprintf(stderr, "Error al intentar acceder a %s: ", pieces[i]);  // Mensaje de error
                perror("");
                continue;       //Sale del bucle aqui y procesa la siguiente iteración
            }
            printf("************%s\n",pieces[i]);
            if (strcmp(pieces[1],"-long") == 0) {
                while ((entry = readdir(dir)) != NULL) {        //Obtiene la información de todos los archivos del directorio
                    struct stat data;
                    if (GetFileData(hide,&data,pieces[i],entry->d_name)) {
                        time_t t = data.st_atim.tv_sec;         //Almacenamos la hora en segundos desde 1970
                        struct tm *tm_info = localtime(&t);
                        char buffer[26];
                        //Formatea la fecha y hora en una cadena
                        strftime(buffer, 26, "%Y/%m/%d-%H:%M", tm_info);
                        char* permissions = GetPermissions(data.st_mode);       //Permisos
                        //Propietario y grupo
                        struct passwd *pw = getpwuid(data.st_uid);
                        struct group *gr = getgrgid(data.st_gid);
                        printf("%s   %ld (%ld) %s %s %s %9ld  %s\n",buffer,data.st_nlink,data.st_ino,pw->pw_name,gr->gr_name,permissions,data.st_size,entry->d_name);
                        free(permissions);
                    }
                }
            }else if (strcmp(pieces[1],"-acc") == 0) {
                while ((entry = readdir(dir)) != NULL) {
                    struct stat data;
                    if (GetFileData(hide,&data,pieces[i],entry->d_name))
                        printf("%9ld  %s\n",data.st_size,entry->d_name);
                }
            }else if (strcmp(pieces[1],"-link") == 0) {
                while ((entry = readdir(dir)) != NULL) {
                    struct stat data;
                    if (GetFileData(hide,&data,pieces[i],entry->d_name)) {
                        char link[LENGTH_MAX];
                        ssize_t linklen = readlink(entry->d_name,link,LENGTH_MAX);
                        if(linklen == -1) {         //No tiene enlace simbólico
                            printf("%9ld  %s\n",data.st_size,entry->d_name);
                        }else {
                            link[linklen] = '\0';  // Añadimos terminador nulo
                            printf("El enlace simbólico '%s' apunta a '%s'\n", entry->d_name, link);
                        }
                    }
                }
            }else if (strcmp(pieces[1],"-hid") == 0) {
                hide = false;
                while ((entry = readdir(dir)) != NULL) {
                    struct stat data;
                    if (GetFileData(hide,&data,pieces[i],entry->d_name))
                        printf("%9ld  %s\n",data.st_size,entry->d_name);
                }
            }else {
                perror("Comando no válido, introduce \"help\" para ver los disponibles");
            }
            closedir(dir);
        }
    }
}


static void listDirectoryRecursively(const char *dirName, bool showHidden, bool showLong, bool showLink, bool showAccessTime) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    //abre el directorio especificado por dirName
    dir = opendir(dirName);
    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    //imprime la cabecera del directorio
    printf("************%s\n", dirName);

    while ((entry = readdir(dir)) != NULL) {
        char fullPath[1024];
        //verifica si se deben mostrar archivos ocultos (que comienzan con '.')
        if (!showHidden && entry->d_name[0] == '.') {
            continue;  //omitir archivos ocultos
        }

        //ruta completa del archivo
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirName, entry->d_name);

        //maneja el caso de showLink: usa lstat si se requiere seguir enlaces simbólicos
        if (showLink) {
            if (lstat(fullPath, &fileStat) != 0) {  //lstat para manejar enlaces simbólicos
                perror("Error en lstat");
                continue;
            }
        } else {
            if (stat(fullPath, &fileStat) != 0) {  //stat normal si no queremos manejar enlaces simbólicos
                perror("Error en stat");
                continue;
            }
        }

        //maneja el caso de showAccessTime: imprime la fecha de acceso
        if (showAccessTime) {
            if (S_ISDIR(fileStat.st_mode)) {
                printf("%10ld  %s\n", fileStat.st_size, entry->d_name);
            } else {
                printf("%10ld  %s\n", fileStat.st_size, entry->d_name);
            }
        }

        //maneja el caso de showLong, imprime los detalles largos
        if (showLong) {
            //modo (permisos y tipo de archivo)
            char *permissions = GetPermissions(fileStat.st_mode);

            //propietario y grupo
            struct passwd *pw = getpwuid(fileStat.st_uid);
            struct group *gr = getgrgid(fileStat.st_gid);

            //fecha (última modificación)
            char timebuf[80];
            struct tm *timeinfo = localtime(&fileStat.st_mtime); // Última modificación
            strftime(timebuf, sizeof(timebuf), "%Y/%m/%d-%H:%M", timeinfo);

            //imprime en formato largo
            printf("%s %ld (%7ld) %8s %8s %10ld %s %s\n",
                   timebuf,               //Fecha y hora
                   fileStat.st_nlink,     //Número de enlaces
                   (long)fileStat.st_ino, //Número de inodo
                   pw ? pw->pw_name : "-", //Propietario
                   gr ? gr->gr_name : "-", //Grupo
                   fileStat.st_size,      //Tamaño del archivo
                   permissions,           //Permisos
                   entry->d_name          //Nombre del archivo
            );
            free(permissions); //libera la memoria de los permisos

        }else {
            //imprime el nombre del archivo y el tamano si no es formato largo
            printf("%10ld  %s\n", fileStat.st_size, entry->d_name);
        }

        //recursividad para subdirectorios (omitir '.' y '..')
        if (S_ISDIR(fileStat.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            listDirectoryRecursively(fullPath, showHidden, showLong, showLink, showAccessTime);
        }
    }

    //cierra el directorio
    closedir(dir);
}

void options(char *pieces[], bool *showHidden, bool *showLong, bool *showLink, bool *showAccessTime, int *i) {
    *showHidden = false;  // si se deben mostrar los archivos ocultos
    *showLong = false;    // si se debe mostrar información detallada
    *showLink = false;    // si se deben mostrar los enlaces simbólicos
    *showAccessTime = false;  // si se deben mostrar las fechas de acceso
    *i = 1;

    // analiza las opciones
    for (; pieces[*i] != NULL && pieces[*i][0] == '-'; (*i)++) {  // incrementa i correctamente
        if (strcmp(pieces[*i], "-hid") == 0)
            *showHidden = true;
        else if (strcmp(pieces[*i], "-long") == 0)
            *showLong = true;
        else if (strcmp(pieces[*i], "-link") == 0)
            *showLink = true;
        else if (strcmp(pieces[*i], "-acc") == 0)
            *showAccessTime = true;
        else {
            fprintf(stderr, "Opción no reconocida: %s\n", pieces[*i]);
            return;
        }
    }

    // Si no se especifican directorios, asigna el directorio actual
    if (pieces[*i] == NULL) {
        char cwd[LENGTH_MAX_INPUT];
        if (getcwd(cwd, LENGTH_MAX_INPUT) != NULL) {
            pieces[*i] = strdup(cwd);  // asigna el directorio actual a pieces[i]
            pieces[*i + 1] = NULL;     // marca el final de las piezas
        } else {
            perror("Error obteniendo el directorio actual");
        }
    }
}



//Intepreta los argumentos y llama a listDirectoryRecursively con los parametros adecuados
void command_reclist(char *pieces[]) {
    bool showHidden, showLong, showLink, showAccessTime;
    int i;
    options(pieces, &showHidden, &showLong, &showLink, &showAccessTime, &i);

    //Listar directorios recursivamente
    while (pieces[i] != NULL) {
        listDirectoryRecursively(pieces[i], showHidden, showLong, showLink, showAccessTime);
        i++;
    }
}


/*listrev [-hid][-long][-link][-acc] n1 n2 ..	lista recursivamente contenidos de directorios (subdirectorios antes
    -hid: incluye los ficheros ocultos
    -acc: acesstime
    -link: si es enlace simbolico, el path contenido
    listrev -long /home/pablojhd/Escritorio/SO/P01/cmake-build-debug*/


static void listDirectoryRecursivelyReverse(const char *dirName, bool showHidden, bool showLong, bool showLink, bool showAccessTime){
    DIR *dir;
    struct dirent *entry;
    //se abre el directorio especidifcado por dirName
    dir = opendir(dirName);
    if (dir == NULL) {
        fprintf(stderr, "Error al intentar acceder a %s: ", dirName);  // Mensaje de error
        perror("");
        return;
    }

    char fullPath[LENGTH_MAX_INPUT];
    struct stat fileData;

    while ((entry = readdir(dir)) != NULL) {
        if ((!showHidden && entry->d_name[0] == '.') || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {       //No se les aplica ya que al leer este tipo de directorios e intentar acceder a los subdirectorios pueden provocar una recursión infinita al seguir llamando a la misma carpeta o a sus padres.
            continue;  // Omitir archivos ocultos
        }
        snprintf(fullPath, LENGTH_MAX_INPUT, "%s/%s", dirName, entry->d_name); //ruta completa del archivo
        if (stat(fullPath, &fileData) == -1) {
            fprintf(stderr, "Error al intentar acceder a %s: ", entry->d_name);  // Mensaje de error
            perror("");
            continue;
        }
        if (S_ISDIR(fileData.st_mode)) {
            listDirectoryRecursivelyReverse(fullPath, showHidden, showLong, showLink, showAccessTime);
        }
    }

    // Ahora que hemos recorrido recursivamente, volvemos a listar el contenido del directorio actual
    rewinddir(dir);  // Volver al inicio del directorio para la segunda pasada (evitar cerrar/abrir)
    printf("************%s\n", dirName);  // Imprimir el nombre del directorio

    while ((entry = readdir(dir)) != NULL) {
        if (!showHidden && (entry->d_name[0] == '.' || strcmp(entry->d_name, "..") == 0)) {
            continue;  // Omitir archivos ocultos y el propio directorio/parent
        }

        // Generar la ruta completa del archivo para luego pasársela a stat ya que si solo le pasamos el nombre no será capaz de acceder porque en ese momento no se encontrará en su directorio
        snprintf(fullPath, LENGTH_MAX_INPUT, "%s/%s", dirName, entry->d_name);

        if (stat(fullPath, &fileData) == -1) {
            printf("Error \n");
            fprintf(stderr, "Error al intentar acceder a %s: ", entry->d_name);  // Mensaje de error
            perror("");
            continue;
        }
        // Mostrar información detallada si está habilitado
        if (showLong) {
            time_t t = fileData.st_atim.tv_sec;         //Almacenamos la hora en segundos desde 1970
            struct tm *tm_info = localtime(&t);
            char buffer[26];
            strftime(buffer, 26, "%Y/%m/%d-%H:%M", tm_info);

            char* permissions = GetPermissions(fileData.st_mode);  // Obtener permisos
            struct passwd *pw = getpwuid(fileData.st_uid);  // Propietario
            struct group *gr = getgrgid(fileData.st_gid);  // Grupo

            printf("%s   %ld (%ld) %s %s %s %9ld  %s\n",buffer,fileData.st_nlink,fileData.st_ino,pw->pw_name,gr->gr_name,permissions,fileData.st_size,entry->d_name);

            free(permissions);
        }else if (showLink || showAccessTime)
            printf("%9ld  %s\n",fileData.st_size,entry->d_name);
        else {
            char link[LENGTH_MAX];
            ssize_t linklen = readlink(entry->d_name,link,LENGTH_MAX);
            if(linklen == -1) {         //No tiene enlace simbólico
                printf("%9ld  %s\n",fileData.st_size,entry->d_name);
            }else {
                link[linklen] = '\0';  // Añadimos terminador nulo
                printf("El enlace simbólico '%s' apunta a '%s'\n", entry->d_name, link);
            }
        }
    }

    closedir(dir);  // Cerrar el directorio
}



void command_revlist(char *pieces[]) {
    bool showHidden, showLong, showLink, showAccessTime;
    int i;
    options(pieces, &showHidden, &showLong, &showLink, &showAccessTime, &i);

        //Listar directorios recursivamente
        while (pieces[i] != NULL) {
            listDirectoryRecursivelyReverse(pieces[i], showHidden, showLong, showLink, showAccessTime);
            i++;
        }
}

//Borra ficheros o directorios vacios
void command_erase(char *pieces[]) {
    struct stat fileStat;

    // Recorre todos los argumentos que se pasan al comando
    for (int i = 1; pieces[i] != NULL; i++) {  // Cambiado a verificar que pieces[i] no sea NULL
        // Comprueba si el archivo o directorio existe
        if (stat(pieces[i], &fileStat) == 0) {
            // Si es un fichero regular, lo borra
            if (S_ISREG(fileStat.st_mode)) {
                if (unlink(pieces[i]) == 0) {
                    printf("El fichero '%s' ha sido borrado.\n", pieces[i]);
                } else {
                    perror("Error al borrar el fichero");
                }
            }
            // Si es un directorio y está vacío, intenta borrarlo
            else if (S_ISDIR(fileStat.st_mode)) {
                DIR *dir = opendir(pieces[i]);
                if (dir == NULL) {
                    perror("Error al abrir el directorio");
                    continue;
                }

                struct dirent *entry;
                int isEmpty = 1;  // Asume que el directorio está vacío

                // Recorre el contenido del directorio para comprobar si está vacío
                while ((entry = readdir(dir)) != NULL) {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        isEmpty = 0;  // El directorio no está vacío
                        break;
                    }
                }

                closedir(dir);

                // Si el directorio está vacío, intenta eliminarlo
                if (isEmpty) {
                    if (rmdir(pieces[i]) == 0) {
                        printf("El directorio vacío '%s' ha sido borrado.\n", pieces[i]);
                    } else {
                        perror("Error al borrar el directorio");
                    }
                } else {
                    printf("El directorio '%s' no está vacío y no puede ser borrado.\n", pieces[i]);
                }
            } else {
                printf("'%s' no es un fichero regular ni un directorio.\n", pieces[i]);
            }
        } else {
            // Si no existe el archivo o directorio, muestra un mensaje de error
            printf("El fichero o directorio '%s' no existe.\n", pieces[i]);
        }
    }
}

static bool isDirectory(char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        fprintf(stderr, "Error al intentar acceder a %s: ", path);  // Mensaje de error
        perror("");
        return false;
    }
    return S_ISDIR(st.st_mode);
}

//delrec [name1 name2 ..]	Borra ficheros o directorios no vacios recursivamente

//Acabar, devuelve un bool si ha podido borrar bien el directorio o fichero
static bool deleteRecursively(char *name) {
    char fullPath[LENGTH_MAX_INPUT];
    // Determinar si la ruta es absoluta o relativa
    if (name[0] == '/') {
        // Ruta absoluta
        strncpy(fullPath, name, LENGTH_MAX_INPUT - 1);
        fullPath[LENGTH_MAX_INPUT - 1] = '\0';  // Asegurar terminación nula
    } else {
        // Ruta relativa: obtener el directorio actual y concatenar
        char cwd[LENGTH_MAX_INPUT];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            fprintf(stderr, "Error al obtener el directorio actual: %s",strerror(errno));
            return false;
        }
        snprintf(fullPath, LENGTH_MAX_INPUT, "%s/%s", cwd, name);
    }

    if (isDirectory(fullPath)) {        // Si es un directorio, lo eliminamos recursivamente
        DIR *dir = opendir(fullPath);
        if (dir == NULL) {
            fprintf(stderr, "Error al intentar acceder a %s: %s\n", fullPath,strerror(errno));  // Mensaje de error
            return false;
        }

        struct dirent *entry;
        char subPath[LENGTH_MAX_INPUT];

        // Leemos el contenido del directorio
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            // Construimos la ruta completa de los archivos o subdirectorios
            snprintf(subPath, LENGTH_MAX_INPUT, "%s/%s", fullPath, entry->d_name);

            // Eliminamos recursivamente el contenido del directorio
            if (!deleteRecursively(subPath)) {
                closedir(dir);
                return false;
            }
        }

        closedir(dir);

        // Una vez vacío, eliminamos el propio directorio
        if (rmdir(fullPath) == -1) {
            fprintf(stderr, "Error al eliminar directorio %s: %s", fullPath,strerror(errno)); //errno almacena el código de error de la última operación que falló.
            return false;
        }
    }else {         // Si es un archivo regular o un enlace simbólico, lo eliminamos
        if (unlink(fullPath) == -1) {
            fprintf(stderr, "Error al eliminar el archivo %s: %s", name,strerror(errno));  // Mensaje de error
            fflush(stdout);
            return false;
        }
    }

    return true;
}


void command_delrec (char *pieces[]){
    if (pieces[1] == NULL) {
        // Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            fprintf(stderr, "Error al obtener el directorio actual: %s",strerror(errno));
        }
    }else {
        for (int i = 1; pieces[i] != NULL; ++i) {
            if(deleteRecursively(pieces[i])) {
                printf("%s eliminado\n", pieces[i]);    //No ponerlo en la final
            }
        }
    }
}