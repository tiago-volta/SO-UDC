
#include "Commands.h"

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
        char cwd[LENGTH_MAX_PATH];
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
static void repeatCommand(tPosH p,bool *finished, CommandListC *commandList, HistoryList *history, OpenFileList *openFileList){
    char *trozos[LENGTH_MAX_PHRASE];
    tItemH cadena;
    tItemH *comando = getItemH(p, history);
    printf("Ejecutando historic (%d): %s\n",p,*comando);
    strcpy(cadena,*comando);
    SplitString(*comando,trozos);
    processInput(finished,&cadena,trozos,commandList,history,openFileList);
}

//Mustra el historial de comandos introducidos, o repite un comando ya introducido o imprimer los últimos n comandos
void command_historic (char *pieces[],bool *finished,CommandListC *commandList, HistoryList *history, OpenFileList *openFileList) {
    char *NoValidCharacter;
    if (pieces[1] != NULL) {
        const long int n = strtol(pieces[1],&NoValidCharacter,10);         //Variable para almacenar el n si lo hay
        int number = (int) n;
        if(*NoValidCharacter == '\0') {
            if (0 <= number && number <= lastH(*history)) {
                repeatCommand(number,finished,commandList,history,openFileList);
            }else if (number < 0) {
                number = -number;       //Cambiar el signo
                printLastNH(history,number);
            }else
                perror("No se han ejecutado comandos suficientes");
        }else if(NoValidCharacter == pieces[1]) {
            perror("No se introdujo ningún número válido.\n");
        }else{
            printf("Parte de la cadena no es válida: %s\n", NoValidCharacter);
            printf("Parte numérica leída: %d\n", number);
            if (0 <= number && number <= lastH(*history)) {
                repeatCommand(number,finished,commandList,history,openFileList);
            }else if (number < 0) {
                number = -number;       //Cambiar el signo
                printLastNH(history,number);
            }else
                perror("No se han ejecutado comandos suficientes");
            }
    }else {
        printListH(history);
    }
}

//Función que abre un archivo con un modo específico
void command_open(char *pieces[],OpenFileList *openFileList) {
    int i, df, mode = 0;

    // Si no se especificó un archivo, listar los archivos abiertos
    if (pieces[1] == NULL) {
        printListF(*openFileList);  // Lista los archivos abiertos
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
        tItemF item = defineItem(df,mode,pieces[1]);
        if(insertItemF(item,openFileList))          // Añade el archivo a la lista de archivos abiertos
            printf("Archivo abierto: descriptor %d, archivo %s, modo %d\n", df, pieces[1], mode);
    }
}

//Función que cierra un archivo a partir de su descriptor
void command_close(char *pieces[],OpenFileList *openFileList) {
    int df;

    // Si no se especifica un descriptor o el descriptor es menor que 0, listar los archivos abiertos
    if (pieces[1] == NULL || (df = atoi(pieces[1])) < 0) {
        printListF(*openFileList);  // Lista los archivos abiertos
        return;
    }

    // Intentar cerrar el archivo con el descriptor especificado
    if (close(df) == -1) {
        perror("Imposible cerrar archivo");
    } else {
        tPosF p= findFile(df,*openFileList);  // Elimina el archivo de la lista de archivos abiertos
        deleteOpenFileAtPositionF(p,openFileList);
        printf("Archivo cerrado: descriptor %d, archivo %s\n", df, pieces[1]);
    }
}

//Función que duplica un archivo a partir de du identificador
void command_dup(char *pieces[], OpenFileList *openFileList) {
    int df, duplicated;
    char aux[MAXNAME], *p;
    tItemF item;

    // Si no hay un descriptor válido, lista los archivos abiertos
    if (pieces[1] == NULL || (df = atoi(pieces[1])) < 0) {
        printListF(*openFileList);  // Lista los archivos abiertos
        return;
    }
    printf("pieces[1]: %s\n", pieces[1]);
    // Duplica el descriptor
    if ((duplicated = dup(df)) == -1) {
        perror("Imposible duplicar descriptor");
        return;
    }
    // Obtiene el nombre del archivo asociado con el descriptor original
    p = getItemF(findFile(df,*openFileList)).name;
    // Prepara una cadena para mostrar la información del duplicado
    if (snprintf(aux, MAXNAME, "dup %d (%s)", df, p) >= (int)sizeof(aux)) {
        fprintf(stderr, "Error: El mensaje a imprimir fue truncado.\n");
    }
    item = defineItem(df,fcntl(duplicated, F_GETFL),aux);
    if(insertItemF(item,openFileList))                  // Añade el nuevo descriptor duplicado a la lista de archivos abiertos
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
void command_help(char * pieces[], CommandListC *commandList) {
    if (pieces[1] != NULL) {
        if (strcmp(pieces[1],"-all") == 0) {
            printCommandDescriptionListC(*commandList);
            return;
        }
        int pos = FindCommandC(commandList,pieces[1]);
        if(pos != CNULL) {
            tCommandC command= getCommandC(pos,*commandList);
            printf("%s %s\n", command.name,command.description);
            return;
        }
        perror("Comando no válido, introduce ""help"" para ver los disponibles");
    }else{
        printCommandListC(*commandList);
    }
}

//Comando que limpia todas las listas, cierra los archivos y finaliza el programa
void command_exit(bool *finished,OpenFileList *openFileList, HistoryList *history, CommandListC *commandList) {
    // Cierra todos los archives abiertos
    for (tPosF i = firstF(*openFileList); i != FNULL; i = nextF(i)) {
        if (close(getItemF(i).df) == -1) {
            perror("Error al cerrar el archivo");
        }
    }
    // Limpiamos las listas utilizadas en el programa
    CleanCommandListC(commandList);
    CleanListF(openFileList);
    CleanListH(history);

    // Establece una bandera para indicar que la shell debe terminar
    *finished = true;

    // Imprime un mensaje de despedida
    printf("Saliendo de la shell...\n");
}

//Sino se le pasa argumento imprime el directorio actual, si se le pasa argumento crea un archivo con ese nomnbre, si ya existe no lo crea
void command_makefile(char *pieces[]) {
    if (pieces[1] == NULL) {                            // Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX_PATH];
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
        char cwd[LENGTH_MAX_PATH];
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
                char link[LENGTH_MAX_PHRASE];
                ssize_t linklen = readlink(pieces[i],link,LENGTH_MAX_PHRASE);
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
    char cwd[LENGTH_MAX_PATH]; //buffer para almacenar el directorio actual

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
        char cwd[LENGTH_MAX_PATH];
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
                        char link[LENGTH_MAX_PHRASE];
                        ssize_t linklen = readlink(entry->d_name,link,LENGTH_MAX_PHRASE);
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
    char fullPath[LENGTH_MAX_INPUT];

    //abre el directorio especificado por dirName
    dir = opendir(dirName);
    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    //imprime la cabecera del directorio
    printf("************%s\n", dirName);

    while ((entry = readdir(dir)) != NULL) {
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


//Se puede fusionar con la reversa porque son iguales, si tiempo hacerlo

//Intepreta los argumentos y llama a listDirectoryRecursively cpn los parametros adecuados
void command_reclist(char *pieces[]) {
    bool showHidden = false; //si se deben mostrar los archivos ocultos
    bool showLong = false; //si se debe mostrar información detallada
    bool showLink = false; //si se deben mostrar los enlaces simbólicos
    bool showAccessTime = false; //si se deben mostrar las fechas de acceso
    int i = 1;

    //analiza las opciones
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
            fprintf(stderr, "Opción no reconocida: %s\n", pieces[i]);
            return;
        }
    }

    //Si no se especifican directorios imprimimos el directorio actual, si te fijas en la de referencia hacen eso
    if (pieces[i] == NULL) {
        // Si no se especifica un directorio, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX_INPUT];
        if (getcwd(cwd, LENGTH_MAX_INPUT) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    }
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

//Perfecta, modulizar en todo caso

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
            char link[LENGTH_MAX_PHRASE];
            ssize_t linklen = readlink(entry->d_name,link,LENGTH_MAX_PHRASE);
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
        bool showHidden = false; //si se deben mostrar los archivos ocultos
        bool showLong = false; //si se debe mostrar información detallada
        bool showLink = false; //si se deben mostrar los enlaces simbólicos
        bool showAccessTime = false; //si se deben mostrar las fechas de acceso
        int i = 1;

        //Analiza   las opciones
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
                    printf("Imposible borrar '%s' no está vacío y no puede ser borrado.\n", pieces[i]);
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
    char fullPath[LENGTH_MAX_FULLPATH];
    // Determinar si la ruta es absoluta o relativa
    if (name[0] == '/') {
        // Ruta absoluta
        strncpy(fullPath, name, LENGTH_MAX_FULLPATH - 1);
        fullPath[LENGTH_MAX_FULLPATH - 1] = '\0';  // Asegurar terminación nula
    } else {
        // Ruta relativa: obtener el directorio actual y concatenar
        char cwd[LENGTH_MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            fprintf(stderr, "Error al obtener el directorio actual: %s",strerror(errno));
            return false;
        }
        snprintf(fullPath, LENGTH_MAX_FULLPATH, "%s/%s", cwd, name);
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
        char cwd[LENGTH_MAX_PHRASE];
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