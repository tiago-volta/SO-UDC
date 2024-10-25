/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Práctica 1
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 25 / 10 / 24
 */

#include "Commands.h"
void printInfo(const char *fileName, struct stat *fileData, bool showLong, bool showLink, bool showAccessTime);
static void listDirectoryRecursivelyUnified(const char *dirName, bool showHidden, bool showLong, bool showLink, bool showAccessTime, bool reverseOrder);
void processDirectoryContent(DIR *dir, const char *dirName, bool showHidden, bool showLong, bool showLink, bool showAccessTime, bool reverseOrder);
void print_file_info(char *name, struct stat *data, char *option);

//Imprime el nombre y/o los logins de los autores
void command_authors(char * pieces[]) {
    const char * names[] = {"Pablo Herrero","Tiago Volta"};
    const char * logins[] = {"pablo.herrero.diaz","tiago.velosoevolta"};
    int NumAuthors = 2;

    //Si no se especifica opción o se usa la opción "-n", imprime los nombres de los autores
    if(pieces[1] == NULL || strcmp(pieces[1],"-n")==0) {
        printf("Nombres de los autores: \n");
        for(int i=0; i<NumAuthors; i++) {
            printf("%s\n",names[i]);
        }
    }

    //Si no se especifica opción o se usa la opción "-l", imprime los logins de los autores
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
    if (pieces[1] == NULL) {                            //Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    } else {                                            //Cambia el directorio de trabajo actual al directorio especificado
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

    //Obtiene la hora actual
    time(&t);
    tm_info = localtime(&t);

    //Si no hay argumentos o si se especifica "-t", imprime solo la hora
    if (pieces[1] == NULL || strcmp(pieces[1], "-t") == 0) {
        strftime(buffer, 26, "%H:%M:%S", tm_info);
        printf("Hora actual: %s\n", buffer);
    }

    //Si no hay argumentos o si se especifica "-d", imprime solo la fecha
    if (pieces[1] == NULL || strcmp(pieces[1], "-d") == 0) {
        strftime(buffer, 26, "%Y-%m-%d", tm_info);
        printf("Fecha actual: %s\n", buffer);
    }
}

//Función auxiliar para repetir un comando guardado en el historial
static void repeatCommand(tPosH p,bool *finished, CommandListC *commandList, HistoryList *history, OpenFileList *openFileList){
    char *trozos[LENGTH_MAX_PHRASE];
    tItemH cadena;  //Cadena que almacena el comando a repetir
    tItemH *comando = getItemH(p, history);  //Obtiene el comando a repetir
    printf("Ejecutando historic (%d): %s\n",p,*comando);
    strcpy(cadena,*comando);  //Copia el comando a cadena
    SplitString(*comando,trozos);  //Separa el comando en trozos
    processInput(finished,&cadena,trozos,commandList,history,openFileList);  //Procesa el comando
}

//Muestra el historial de comandos introducidos, repite un comando o imprime los últimos n comandos
void command_historic (char *pieces[],bool *finished,CommandListC *commandList, HistoryList *history, OpenFileList *openFileList) {
    char *NoValidCharacter;

    //Si se proporciona un argumento numérico
    if (pieces[1] != NULL) {
        const long int n = strtol(pieces[1],&NoValidCharacter,10);         //Variable para almacenar el número si lo hay
        int number = (int) n; //(int) convierte el long int a int, es necesario por el strtol que retorna un long int

        if(*NoValidCharacter == '\0') {  //Si todo el argumento es un número válido
            if (0 <= number && number <= lastH(*history)) {
                repeatCommand(number,finished,commandList,history,openFileList);
            } else if (number < 0) {  //Si el número es negativo
                number = -number;       //Cambiar el signo
                printLastNH(history,number);  //Imprime los últimos n comandos
            } else {
                perror("No se han ejecutado comandos suficientes");
            }
        } else if(NoValidCharacter == pieces[1]) {  //Si no se introdujo ningún número válido
            perror("No se introdujo ningún número válido.\n");
        } else {  //Parte de la cadena es válida pero hay un error
            printf("Parte de la cadena no es válida: %s\n", NoValidCharacter);
            printf("Parte numérica leída: %d\n", number);
            if (0 <= number && number <= lastH(*history)) {  //Si el número es válido
                repeatCommand(number,finished,commandList,history,openFileList);
            } else if (number < 0) {
                number = -number;       //Cambiar el signo
                printLastNH(history,number);  //Imprime los últimos n comandos
            } else {
                perror("No se han ejecutado comandos suficientes");
            }
        }
    } else {
        printListH(history);  //Si no se proporciona argumento, imprime todo el historial
    }
}

//Función que abre un archivo con un modo específico
void command_open(char *pieces[],OpenFileList *openFileList) {
    int i, df, mode = 0;

    //Si no se especificó un archivo, listar los archivos abiertos
    if (pieces[1] == NULL) {
        printListF(*openFileList);  //Lista los archivos abiertos
        return;
    }

    //Procesar los parámetros para determinar el modo de apertura del archivo
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

    //Intentar abrir el archivo con el modo especificado
    if ((df = open(pieces[1], mode, 0777)) == -1) {   //0777 es el modo de apertura, si es -1 es que ha habido un error
        perror("Imposible abrir archivo");
    } else {
        tItemF item = defineItem(df,mode,pieces[1]);  //Define un item con el descriptor, el modo y el nombre del archivo
        if(insertItemF(item,openFileList))          //Añade el archivo a la lista de archivos abiertos
            printf("Archivo abierto: descriptor %d, archivo %s, modo %d\n", df, pieces[1], mode);
    }
}

//Función que cierra un archivo a partir de su descriptor
void command_close(char *pieces[],OpenFileList *openFileList) {
    int df;

    //Si no se especifica un descriptor o el descriptor es menor que 0, lista los archivos abiertos
    if (pieces[1] == NULL || (df = atoi(pieces[1])) < 0) {
        printListF(*openFileList);  //Lista los archivos abiertos
        return;
    }

    //Intentar cerrar el archivo con el descriptor especificado
    if (close(df) == -1) {
        perror("Imposible cerrar archivo");
    } else {
        tPosF p= findFile(df,*openFileList);  //Busca el archivo en la lista de archivos abiertos
        deleteOpenFileAtPositionF(p,openFileList);  //Elimina el archivo de la lista de archivos abiertos
        printf("Archivo cerrado: descriptor %d, archivo %s\n", df, pieces[1]);
    }
}

//Función que duplica un archivo a partir de su identificador
void command_dup(char *pieces[], OpenFileList *openFileList) {
    int df, duplicated;
    char aux[MAXNAME], *p;
    tItemF item;

    //Si no hay un descriptor válido, lista los archivos abiertos
    if (pieces[1] == NULL || (df = atoi(pieces[1])) < 0) {
        printListF(*openFileList);  //Lista los archivos abiertos
        return;
    }

    //Intenta duplicar el descriptor
    if ((duplicated = dup(df)) == -1) {  //Si no se puede duplicar el descriptor, muestra un error
        perror("Imposible duplicar descriptor");
        return;
    }

    //Obtiene el nombre del archivo asociado con el descriptor original
    p = getItemF(findFile(df,*openFileList)).name;

    //Prepara una cadena para mostrar la información del duplicado
    if (snprintf(aux, MAXNAME, "dup %d (%s)", df, p) >= 0) {
        printf("Descriptor duplicado %d -> %d\n",df,duplicated);  //Imprime los descriptores
        item= defineItem(duplicated, O_RDWR, aux);  //Define el nuevo archivo duplicado
        insertItemF(item, openFileList);  //Inserta el nuevo archivo en la lista de archivos abiertos
    }
}

//Función que muestra la información del sistema
void command_infosys() {
    struct utsname sysinfo;  //Estructura utsname para almacenar la información del sistema

    //Obtiene la información del sistema a través de uname
    if (uname(&sysinfo) == -1) {
        perror("Error al obtener la información del sistema");
        return;
    }
    printf("%s (%s), OS: %s - %s - %s \n",sysinfo.nodename,sysinfo.machine,sysinfo.sysname,sysinfo.release,sysinfo.version);
}

//Función que muestra los comandos disponibles
void command_help(char * pieces[], CommandListC *commandList) {
    //Si se pasa un argumento con el comando help
    if (pieces[1] != NULL) {
        //Si el argumento es "-all"
        if (strcmp(pieces[1],"-all") == 0) {
            printCommandDescriptionListC(*commandList);  //Muestra la descripción de todos los comandos
            return;
        }
        //Busca la posición del comando en la lista de comandos
        int pos = FindCommandC(commandList,pieces[1]);
        if(pos != CNULL) {  //Si encuentra el comando, lo imprime
            tCommandC command = getCommandC(pos,*commandList);
            printf("%s %s\n", command.name,command.description);  //Imprime el nombre y descripción del comando
            return;
        }
        //Si el comando no es válido, imprime un error
        perror("Comando no válido, introduce ""help"" para ver los disponibles");
    } else {
        //Si no se pasa ningún argumento, imprime solo la lista de comandos disponibles
        printCommandListC(*commandList);  //Muestra los comandos sin sus descripciones
    }
}

//Función que limpia todas las listas, cierra los archivos y finaliza el programa
void command_exit(bool *finished, OpenFileList *openFileList, HistoryList *history, CommandListC *commandList) {
    //Cierra todos los archivos abiertos
    for (tPosF i = firstF(*openFileList); i != FNULL; i = nextF(i)) {
        if (close(getItemF(i).df) == -1) {  //Verifica si hubo un error al cerrar el archivo
            perror("Error al cerrar el archivo");
        }
    }
    //Limpia las listas utilizadas en el programa
    CleanCommandListC(commandList);  //Limpia la lista de comandos
    CleanListF(openFileList);        //Limpia la lista de archivos abiertos
    CleanListH(history);             //Limpia el historial de comandos

    //Establece una bandera para indicar que la shell debe terminar
    *finished = true;  //Cambia la bandera finished a true para terminar la ejecución

    //Imprime un mensaje de despedida
    printf("Saliendo de la shell...\n");  //Mensaje de salida
}

//Función que crea un archivo nuevo
void command_makefile(char *pieces[]) {
    //Si no se pasa ningún argumento, imprime el directorio de trabajo actual
    if (pieces[1] == NULL) {
        char cwd[LENGTH_MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {  //Obtiene el directorio de trabajo actual
            printf("Directorio actual: %s\n", cwd);  //Imprime el directorio actual
        } else {
            perror("Error obteniendo el directorio actual");  //Imprime un error si no puede obtener el directorio
        }
    } else {
        //Si el archivo ya existe, imprime un mensaje de error
        if (access(pieces[1], F_OK) != -1) {  //Verifica si el archivo ya existe
            printf("Error: El archivo '%s' ya existe.\n", pieces[1]);  //Mensaje de error si ya existe
        } else {
            //Crea un archivo nuevo si no existe
            FILE *logFile = fopen(pieces[1], "w");  //Abre el archivo en modo escritura
            if (logFile != NULL && fclose(logFile) != EOF) {  //Verifica si se pudo crear y cerrar el archivo
                printf("Archivo '%s' creado correctamente.\n", pieces[1]);  //Mensaje de confirmación si se creó correctamente
            } else
                perror("Error al cerrar el archivo");  //Imprime un error si no se pudo cerrar el archivo
        }
    }
}

//Función estática que determina la letra representativa del tipo de archivo según su modo (mode_t)
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

//Función estática que obtiene los permisos de un archivo en formato de cadena de texto
static char *GetPermissions(mode_t m) {
    char *permisos;
    //Reserva memoria para almacenar los permisos (10 caracteres + espacio extra) y comprueba si la asignación fue exitosa
    if ((permisos=(char *) malloc (12))==NULL)
        return NULL;
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';     //Permiso de lectura por el propietario           /*Como esta función utiliza malloc, hace con que la función sea autónoma
    if (m&S_IWUSR) permisos[2]='w';     //Permiso de escritura por el propietario           en quanto a la gestión de memoria, no necesita de preocuparse por la asignación
    if (m&S_IXUSR) permisos[3]='x';     //Permiso de ejecución por el propietario           de memoria previa.
    if (m&S_IRGRP) permisos[4]='r';     //Permiso de lectura por un grupo                   Evita posibles errores, como no reservar suficiente espacio para los permisos*/
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

//Función que crea un directorio
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

        //Esto creo que no es necesario -- la mantenemos si quieres imprimir los permisos
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

//Función que muestra el directorio actual
void command_cwd() {
    char cwd[LENGTH_MAX_PATH]; //buffer para almacenar el directorio actual

    //Obtiene el directorio actual
    if(getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Directorio actual: %s\n", cwd);
    } else {
        perror("Error obteniendo el directorio actual");
    }
}

//Funcion auxiliar para obtener los datos de los archivos de un directorio
static bool GetFileData(bool hide, struct stat *data, char *dir, char *name) {
    //Si 'hide' es verdadero y el nombre del archivo comienza con '.' o es "..", se oculta
    if (hide && (name[0] == '.' || strcmp(name, "..") == 0)) {
        return false; //No se obtienen datos de archivos ocultos ni del directorio padre
    }
    //Variable para almacenar la ruta completa del archivo (directorio + nombre del archivo)
    char filePath[LENGTH_MAX_INPUT];
    //Construye la ruta completa del archivo utilizando snprintf
    snprintf(filePath, LENGTH_MAX_INPUT, "%s/%s", dir, name);
    //Intenta obtener información del archivo mediante la llamada a stat
    if (stat(filePath, data) == -1) {
        //En caso de error, imprime un mensaje informando el nombre del archivo con problemas
        fprintf(stderr, "Error al intentar acceder a %s: ", name);
        perror("");
        return false; //Retorna false si no se pudo obtener la información del archivo
    }
    //Si se obtiene correctamente la información, retorna true
    return true;
}

// Unified function to list directories and files
void command_list(char *pieces[], bool isDir) {
    if (pieces[2] == NULL) {
        char cwd[LENGTH_MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    } else {
        for (int i = 2; pieces[i] != NULL; i++) {
            if (isDir) {
                DIR *dir;
                struct dirent *entry;
                bool hide = true;

                // Abrir directorio
                dir = opendir(pieces[i]);
                if (dir == NULL) {
                    fprintf(stderr, "Error al intentar acceder a %s: ", pieces[i]);
                    perror("");
                    continue;
                }

                printf("************%s\n", pieces[i]);
                if (strcmp(pieces[1], "-hid") == 0) {
                    hide = false;  // Si es el argumento -hid, mostrar archivos ocultos
                }

                // Iterar sobre los archivos del directorio
                while ((entry = readdir(dir)) != NULL) {
                    struct stat data;
                    if (GetFileData(hide, &data, pieces[i], entry->d_name)) {
                        print_file_info(entry->d_name, &data, pieces[1]);  // Usar la función auxiliar
                    }
                }
                closedir(dir);
            } else {
                struct stat data;
                if (stat(pieces[i], &data) == -1) {
                    fprintf(stderr, "Error al intentar acceder a %s: ", pieces[i]);
                    perror("");
                    continue;
                }
                print_file_info(pieces[i], &data, pieces[1]);  // Usar la función auxiliar
            }
        }
    }
}

// Function to list directory contents
void command_listDir(char *pieces[]) {
    command_list(pieces, true);
}

// Function to list file information
void command_listFile(char *pieces[]) {
    command_list(pieces, false);
}

// Auxiliary function to print file information based on the option
void print_file_info(char *name, struct stat *data, char *option) {
    if (strcmp(option, "-long") == 0) {
        time_t t = data->st_atim.tv_sec;         // Almacenamos la hora en segundos desde 1970
        struct tm *tm_info = localtime(&t);
        char buffer[26];
        strftime(buffer, 26, "%Y/%m/%d-%H:%M", tm_info); // Formatea la fecha y hora en una cadena
        char* permissions = GetPermissions(data->st_mode);  // Obtener permisos en formato legible
        struct passwd *pw = getpwuid(data->st_uid);  // Obtener propietario
        struct group *gr = getgrgid(data->st_gid);   // Obtener grupo
        printf("%s   %ld (%ld) %s %s %s %9ld  %s\n", buffer, data->st_nlink, data->st_ino, pw->pw_name, gr->gr_name, permissions, data->st_size, name);
        free(permissions);  // Liberar memoria
    } else if (strcmp(option, "-acc") == 0) {
        printf("%9ld  %s\n", data->st_size, name);
    } else if (strcmp(option, "-link") == 0) {
        char link[LENGTH_MAX_PHRASE];
        ssize_t linklen = readlink(name, link, LENGTH_MAX_PHRASE);
        if (linklen == -1) {  // No tiene enlace simbólico
            printf("%9ld  %s\n", data->st_size, name);
        } else {
            link[linklen] = '\0';  // Añadimos terminador nulo
            printf("El enlace simbólico '%s' apunta a '%s'\n", name, link);
        }
    } else {
        printf("%9ld  %s\n", data->st_size, name);  // Por defecto, solo imprimir tamaño y nombre
    }
}


// Función que lista directorios de forma recursiva
void command_reclist(char *pieces[]) {
    bool showHidden = false;  // Si se deben mostrar los archivos ocultos
    bool showLong = false;  // Si se debe mostrar información detallada
    bool showLink = false;  // Si se deben mostrar los enlaces simbólicos
    bool showAccessTime = false;  // Si se deben mostrar las fechas de acceso
    bool reverseOrder = false;  // Si se debe listar en orden inverso
    int i = 1;
    // Analiza los argumentos de la línea de comandos
    for (; pieces[i] != NULL && pieces[i][0] == '-'; i++) {
        if (strcmp(pieces[i], "-hid") == 0)
            showHidden = true;
        else if (strcmp(pieces[i], "-long") == 0)
            showLong = true;
        else if (strcmp(pieces[i], "-link") == 0)
            showLink = true;
        else if (strcmp(pieces[i], "-acc") == 0)
            showAccessTime = true;
        else if (strcmp(pieces[i], "-rev") == 0)
            reverseOrder = true;
        else {
            fprintf(stderr, "Opción no reconocida: %s\n", pieces[i]);
            return;
        }
    }
    // Si no se especifican directorios imprimimos el directorio actual
    if (pieces[i] == NULL) {
        char cwd[LENGTH_MAX_INPUT];
        if (getcwd(cwd, LENGTH_MAX_INPUT) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    }
    // Listar directorios recursivamente
    while (pieces[i] != NULL) {
        listDirectoryRecursivelyUnified(pieces[i], showHidden, showLong, showLink, showAccessTime, reverseOrder);
        i++;
    }
}

//Fusionar listDIrectoryRecursivelyReverse y listDirectoryRecursively
void printInfo(const char *fileName, struct stat *fileData, bool showLong, bool showLink, bool showAccessTime) {
    if (showLong) {
        time_t t = fileData->st_atim.tv_sec;
        struct tm *tm_info = localtime(&t);
        char buffer[26];
        strftime(buffer, 26, "%Y/%m/%d-%H:%M", tm_info);

        char* permissions = GetPermissions(fileData->st_mode);
        struct passwd *pw = getpwuid(fileData->st_uid);
        struct group *gr = getgrgid(fileData->st_gid);

        printf("%s   %ld (%ld) %s %s %s %9ld  %s\n", buffer, fileData->st_nlink, fileData->st_ino, pw->pw_name, gr->gr_name, permissions, fileData->st_size, fileName);
        free(permissions);
    } else if (showLink || showAccessTime) {
        char link[LENGTH_MAX_PHRASE];
        ssize_t linklen = readlink(fileName, link, LENGTH_MAX_PHRASE);
        if (linklen == -1) {
            printf("%9ld  %s\n", fileData->st_size, fileName);
        } else {
            link[linklen] = '\0';
            printf("El enlace simbólico '%s' apunta a '%s'\n", fileName, link);
        }
    } else {
        printf("%9ld  %s\n", fileData->st_size, fileName);
    }
}

static void listDirectoryRecursivelyUnified(const char *dirName, bool showHidden, bool showLong, bool showLink, bool showAccessTime, bool reverseOrder) {
    DIR *dir = opendir(dirName);
    if (dir == NULL) {
        fprintf(stderr, "Error al intentar acceder a %s: ", dirName);
        perror("");
        return;
    }
    processDirectoryContent(dir, dirName, showHidden, showLong, showLink, showAccessTime, reverseOrder);
    closedir(dir);
}

void processDirectoryContent(DIR *dir, const char *dirName, bool showHidden, bool showLong, bool showLink, bool showAccessTime, bool reverseOrder) {
    struct dirent *entry;
    struct stat fileData;
    char fullPath[LENGTH_MAX_INPUT];

    if (reverseOrder) {
        while ((entry = readdir(dir)) != NULL) {
            if ((!showHidden && entry->d_name[0] == '.') || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            snprintf(fullPath, LENGTH_MAX_INPUT, "%s/%s", dirName, entry->d_name);
            if (stat(fullPath, &fileData) == -1) {
                fprintf(stderr, "Error al intentar acceder a %s: ", entry->d_name);
                perror("");
                continue;
            }
            if (S_ISDIR(fileData.st_mode)) {
                listDirectoryRecursivelyUnified(fullPath, showHidden, showLong, showLink, showAccessTime, reverseOrder);
            }
        }
        rewinddir(dir);
    }

    printf("************%s\n", dirName);

    while ((entry = readdir(dir)) != NULL) {
        if (!showHidden && (entry->d_name[0] == '.' || strcmp(entry->d_name, "..") == 0)) {
            continue;
        }
        snprintf(fullPath, LENGTH_MAX_INPUT, "%s/%s", dirName, entry->d_name);
        if (stat(fullPath, &fileData) == -1) {
            fprintf(stderr, "Error al intentar acceder a %s: ", entry->d_name);
            perror("");
            continue;
        }
        printInfo(entry->d_name, &fileData, showLong, showLink, showAccessTime);

        if (S_ISDIR(fileData.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            listDirectoryRecursivelyUnified(fullPath, showHidden, showLong, showLink, showAccessTime, reverseOrder);
        }
    }
}
/*se puede decidir si hace en orden normal o en orden inverso a traves de un booleano
Listar en orden normal:
listDirectoryRecursivelyUnified("directorio", true, false, false, false, false);
Listar en orden inverso:
listDirectoryRecursivelyUnified("directorio", true, false, false, false, true);
*/

//Función que lista el contenido de un directorio de forma recursiva en orden inverso
void command_revlist(char *pieces[]) {
        bool showHidden = false;  //Si se deben mostrar los archivos ocultos
        bool showLong = false; //Si se debe mostrar información detallada
        bool showLink = false;  //Si se deben mostrar los enlaces simbólicos
        bool showAccessTime = false;  //Si se deben mostrar las fechas de acceso
        int i = 1;

        //Analiza las opciones
        for (; pieces[i] != NULL && pieces[i][0] == '-'; i++) {
            if (strcmp(pieces[i], "-hid") == 0)
                showHidden = true;
            else if (strcmp(pieces[i], "-long") == 0)
                showLong = true;
            else if (strcmp(pieces[i], "-link") == 0)
                showLink = true;
            else if (strcmp(pieces[i], "-acc") == 0)
                showAccessTime = true;
            else {
                fprintf(stderr, "Opción %s no reconocida:", pieces[i]);
                perror("");
                return;
            }
        }
        if (pieces[i] == NULL) {
            // Si no se especifica un directorio, imprime el directorio de trabajo actual
            char cwd[LENGTH_MAX_PATH];
            if (getcwd(cwd, LENGTH_MAX_PATH) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("Error obteniendo el directorio actual");
            }
        }
        //Listar directorios recursivamente
        while (pieces[i] != NULL) {
            listDirectoryRecursivelyUnified(pieces[i], showHidden, showLong, showLink, showAccessTime, true);
            i++;
        }
}

//Función que borra archivos o directorios vacíos
void command_erase(char *pieces[]) {
    struct stat fileStat;
    //Recorre todos los argumentos que se pasan al comando
    for (int i = 1; pieces[i] != NULL; i++) {  //Cambiado a verificar que pieces[i] no sea NULL
        //Comprueba si el archivo o directorio existe
        if (stat(pieces[i], &fileStat) == 0) {
            //Si es un fichero regular, lo borra
            if (S_ISREG(fileStat.st_mode)) {
                if (unlink(pieces[i]) == 0) {
                    printf("El fichero '%s' ha sido borrado.\n", pieces[i]);
                } else {
                    perror("Error al borrar el fichero");
                }
            }
            //Si es un directorio y está vacío, intenta borrarlo
            else if (S_ISDIR(fileStat.st_mode)) {
                DIR *dir = opendir(pieces[i]);
                if (dir == NULL) {
                    perror("Error al abrir el directorio");
                    continue;
                }

                struct dirent *entry;
                int isEmpty = 1;  //Asume que el directorio está vacío

                //Recorre el contenido del directorio para comprobar si está vacío
                while ((entry = readdir(dir)) != NULL) {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        isEmpty = 0;  //El directorio no está vacío
                        break;
                    }
                }
                closedir(dir);
                //Si el directorio está vacío, intenta eliminarlo
                if (isEmpty) {
                    if (rmdir(pieces[i]) == 0) {
                        printf("El directorio vacío '%s' ha sido borrado.\n", pieces[i]);
                    } else {
                        perror("Error al borrar el directorio");
                    }
                    //Si el directorio no está vacío, muestra un mensaje de error
                } else {
                    printf("Imposible borrar '%s' no está vacío y no puede ser borrado.\n", pieces[i]);
                }
                //Si no es un fichero regular ni un directorio, muestra un mensaje de error
            } else {
                printf("'%s' no es un fichero regular ni un directorio.\n", pieces[i]);
            }
        } else {
            //Si no existe el archivo o directorio, muestra un mensaje de error
            printf("El fichero o directorio '%s' no existe.\n", pieces[i]);
        }
    }
}

//Función auxiliar que verifica si una ruta corresponde a un directorio
static bool isDirectory(char *path) {
    struct stat st;  //Estructura para almacenar información sobre el archivo o directorio
    //Si stat falla (retorna -1), imprime un mensaje de error y retorna 'false'
    if (stat(path, &st) == -1) {
        //Imprimir un mensaje de error detallado en stderr indicando que hubo un problema con 'path'
        fprintf(stderr, "Error al intentar acceder a %s: ", path);
        perror("");  //Añade la razón del error (por ejemplo: "No existe el archivo o directorio")
        return false;  //No es un directorio (o no se pudo acceder)
    }
    //Retorna 'true' si el 'st_mode' indica que la ruta es un directorio, de lo contrario, 'false'
    return S_ISDIR(st.st_mode);
}

//Función auxiliar que elimina un archivo o directorio de forma recursiva
static bool deleteRecursively(char *name) {
    char fullPath[LENGTH_MAX_FULLPATH];  //Almacena la ruta completa del archivo o directorio
    //Determinar si la ruta es absoluta o relativa
    if (name[0] == '/') {
        //Ruta absoluta, simplemente copiamos el nombre a fullPath
        strncpy(fullPath, name, LENGTH_MAX_FULLPATH - 1);
        fullPath[LENGTH_MAX_FULLPATH - 1] = '\0';  //Aseguramos la terminación nula
    } else {
        //Ruta relativa, obtenemos el directorio actual y concatenamos con el nombre
        char cwd[LENGTH_MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {  // Error al obtener el directorio actual
            fprintf(stderr, "Error al obtener el directorio actual: %s", strerror(errno));
            return false;
        }
        snprintf(fullPath, LENGTH_MAX_FULLPATH, "%s/%s", cwd, name);  // Concatenamos
    }
    //Si es un directorio, llamamos recursivamente para eliminar su contenido
    if (isDirectory(fullPath)) {
        DIR *dir = opendir(fullPath);  //Abre el directorio
        if (dir == NULL) {
            //Error al abrir el directorio
            fprintf(stderr, "Error al intentar acceder a %s: %s\n", fullPath, strerror(errno));
            return false;
        }
        struct dirent *entry;
        char subPath[LENGTH_MAX_INPUT];  //Almacena la ruta completa de los archivos en el directorio
        //Leemos el contenido del directorio
        while ((entry = readdir(dir)) != NULL) {
            //Omitimos las entradas especiales '.' y '..' (directorio actual y padre)
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            //Construimos la ruta completa del archivo o subdirectorio
            snprintf(subPath, LENGTH_MAX_INPUT, "%s/%s", fullPath, entry->d_name);
            //Llamada recursiva para eliminar el contenido del subdirectorio
            if (!deleteRecursively(subPath)) {
                closedir(dir);  //Cerramos el directorio en caso de error
                return false;
            }
        }
        closedir(dir);  //Cerramos el directorio después de recorrer su contenido
        //Eliminamos el propio directorio vacío
        if (rmdir(fullPath) == -1) {
            //Error al eliminar el directorio
            fprintf(stderr, "Error al eliminar directorio %s: %s", fullPath, strerror(errno));
            return false;
        }
    } else {
        // Si es un archivo regular o un enlace simbólico, lo eliminamos
        if (unlink(fullPath) == -1) {
            //Error al eliminar el archivo
            fprintf(stderr, "Error al eliminar el archivo %s: %s", name, strerror(errno));
            fflush(stdout);  //Vaciar el búfer de salida estándar
            return false;
        }
    }
    return true;  //Eliminación exitosa
}

//Función que elimina archivos o directorios no vacíos de forma recursiva
void command_delrec(char *pieces[]) {
    if (pieces[1] == NULL) {
        //Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX_PHRASE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            //Error al obtener el directorio actual
            fprintf(stderr, "Error al obtener el directorio actual: %s", strerror(errno));
        }
    } else {
        //Recorre la lista de directorios o archivos a eliminar
        for (int i = 1; pieces[i] != NULL; ++i) {
            //Llama a la función deleteRecursively para eliminar cada archivo o directorio
            if (deleteRecursively(pieces[i])) {
                printf("%s eliminado\n", pieces[i]);  //Mensaje de confirmación de eliminación
            }
        }
    }
}
