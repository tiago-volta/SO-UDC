#include "Commands.h"

int ext_var1, ext_var2, ext_var3;
int ext_init_var1 = 10, ext_init_var2 = 20, ext_init_var3 = 30;  // Valores de ejemplo

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
    printf("PID de shell: %d\n", getpid());
}

//Muestra el PPID del proceso padre
void command_ppid() {
    printf("PID del padre del shell: %d\n", getppid());
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

    //Si no hay argumentos o si se especifica "-t", imprime solo la hora
    if (pieces[1] == NULL || strcmp(pieces[1], "-t") == 0) {
        // Si no hay argumentos o si se especifica "-t", imprime solo la hora
        strftime(buffer, 26, "%H:%M:%S", tm_info);
        printf("Hora actual: %s\n", buffer);
    }

    // Si no hay argumentos o si se especifica "-d", imprime solo la fecha
    if (pieces[1] == NULL || strcmp(pieces[1], "-d") == 0) {
        strftime(buffer, 26, "%Y-%m-%d", tm_info);
        printf("Fecha actual: %s\n", buffer);
    }
}

//Función auxiliar para repetir un comando guardado en el historial
static void repeatCommand(tPosH p, bool *finished, CommandListC *commandList, HistoryList *history, OpenFileList *openFileList, MemoryBlockList *memoryBlockList, ProcessList *processList, DirectoryList *directoryList) {
    char *trozos[LENGTH_MAX_PHRASE];
    char *envp [] = {NULL};
    tItemH cadena;                                                          //Cadena que almacena el comando a repetir
    tItemH *comando = getItemH(p, history);                                 //Obtiene el comando a repetir
    printf("Ejecutando historic (%d): %s\n",p,*comando);
    strcpy(cadena,*comando);                                                //Copia el comando a cadena
    SplitString(*comando,trozos);                                           //Separa el comando en trozos
    processInput(finished,&cadena,trozos, envp, commandList,history,openFileList, memoryBlockList, processList, directoryList); //Procesa el comando
}

//Mustra el historial de comandos introducidos, o repite un comando ya introducido o imprimer los últimos n comandos
void command_historic (char *pieces[],bool *finished,CommandListC *commandList, HistoryList *history, OpenFileList *openFileList, MemoryBlockList *memoryBlockList, ProcessList *processList, DirectoryList *directoryList) {
    char *NoValidCharacter;

    //Si se proporciona un argumento numérico
    if (pieces[1] != NULL) {
        const long int n = strtol(pieces[1],&NoValidCharacter,10);     //Variable para almacenar el número si lo hay
        int number = (int) n;                                               //(int) convierte el long int a int, es necesario por el strtol que retorna un long int

        if(*NoValidCharacter == '\0') {                                     //Si todo el argumento es un número válido, entonces el puntero NoValidCharacter apuntará al \0, strtol convirtió la cadena exitosamente
            if (0 <= number && number <= lastH(*history)) {
                repeatCommand(number,finished,commandList,history,openFileList, memoryBlockList, processList, directoryList);
            }else if (number < 0) {
                number = -number;               //Cambiar el signo
                printLastNH(history,number);    //Imprime los últimos n comandos
            }else
                perror("No se han ejecutado comandos suficientes");
        }else if(NoValidCharacter == pieces[1]) {           //Si no se introdujo ningún número válido el puntero apuntará a la primera posición de pieces[1],
            perror("No se introdujo ningún número válido");
        }else{                                              //Se han introducido carácteres válidos antes de que el puntero apunte a uno no válido
            fprintf(stderr,"Parte de la cadena no es válida: %s\n", NoValidCharacter);
            printf("Parte numérica leída: %d\n", number);   //Se coge la parte que se ha leído exitosamente
            if (0 <= number && number <= lastH(*history)) {
                repeatCommand(number,finished,commandList,history,openFileList, memoryBlockList, processList, directoryList);
            }else if (number < 0) {
                number = -number;                  //Cambiar el signo
                printLastNH(history,number);       //Imprime los últimos n comandos
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
        if (!strcmp(pieces[i], "cr")) mode |= O_CREAT;        //Crea el archivo si no existe.
        else if (!strcmp(pieces[i], "ex")) mode |= O_EXCL;    //Genera un error si el archivo ya existe (solo con O_CREAT).
        else if (!strcmp(pieces[i], "ro")) mode |= O_RDONLY;  //Modo de apertura solo lectura.
        else if (!strcmp(pieces[i], "wo")) mode |= O_WRONLY;  //Modo de apertura solo escritura.
        else if (!strcmp(pieces[i], "rw")) mode |= O_RDWR;    //Modo de apertura para lectura y escritura.
        else if (!strcmp(pieces[i], "ap")) mode |= O_APPEND;  //Abre en modo de adición, escribe al final del archivo.
        else if (!strcmp(pieces[i], "tr")) mode |= O_TRUNC;   //Trunca el archivo a longitud 0 si ya existe (borra contenido).
        else break;
    }


    //Intentar abrir el archivo con el modo especificado
    if ((df = open(pieces[1], mode, 0777)) == -1) {   //Si mode=O_CREAT, open() crea el archivo, y entonces utilizará el tercer parámetro, en este caso 0777, que especifica en octal los permisos de archivo en UNIX, el 0777 significa que todos (propietario, grupo y otros) tienen permisos completos de lectura, escritura y ejecución
        perror("Imposible abrir archivo");
    } else {
        tItemF item = defineItem(df,mode,pieces[1]);  //Define un item con el descriptor, el modo y el nombre del archivo
        if(insertItemF(item,openFileList))            //Añade el archivo a la lista de archivos abiertos
            printf("Archivo abierto: Descriptor: %d -> %s %s\n", df, pieces[1], getFileMode(mode));
        else
            fprintf(stderr,"No se ha podido insertar %s",pieces[1]);
    }
}

//Función que cierra un archivo a partir de su descriptor
void command_close(char *pieces[],OpenFileList *openFileList) {
    int df;
    char *NoValidCharacter, *name;

    // Si no hay un descriptor válido, lista los archivos abiertos
    //Como el operador , asocia a la izquierda entonces se realiza primero la asignación y luego se evalún condiciones sobre esa asignación
    if (pieces[1] == NULL || (df = (int)strtol(pieces[1], &NoValidCharacter, 10), *NoValidCharacter != '\0'||df < 0 )) {        //En el cast podría haber overflow si el número devuelto por strtol es mayor que el rango de un int o pérdida de precisión, pero como los df son valores pequeños no habrá problema
        printListF(*openFileList);  // Lista los archivos abiertos
        return;
    }

    // Intentar cerrar el archivo con el descriptor especificado
    if (close(df) == -1) {
        perror("Imposible cerrar archivo");
    } else {
        tPosF p = findFile(df,*openFileList);  //Busca el archivo en la lista de archivos abiertos
        name = getItemF(p).name;
        deleteOpenFileAtPositionF(p,openFileList);  //Elimina el archivo de la lista de archivos abiertos
        printf("Archivo cerrado: Descriptor: %d -> %s\n", df, name);
    }
}

//Función que duplica un archivo a partir de du descriptor
void command_dup(char *pieces[], OpenFileList *openFileList) {
    int df, duplicated;
    char aux[MAXNAME], *name, *NoValidCharacter;
    tItemF item;

    // Si no hay un descriptor válido, lista los archivos abiertos
    //Como el operador , asocia a la izquierda entonces se realiza primero la asignación y luego se evalún condiciones sobre esa asignación
    if (pieces[1] == NULL ||(df = (int)strtol(pieces[1],&NoValidCharacter,10), *NoValidCharacter != '\0'||df < 0 )) {
        printListF(*openFileList);  // Lista los archivos abiertos
        return;
    }

    //Intenta duplicar el descriptor
    if ((duplicated = dup(df)) == -1) {  //Si no se puede duplicar el descriptor, muestra un error
        perror("Imposible duplicar descriptor");
        return;
    }

    // Obtiene el nombre del archivo asociado con el descriptor original
    name = getItemF(findFile(df,*openFileList)).name;
    int flags = fcntl(duplicated, F_GETFL);      //Obtenemos los flags de estado del archivo, como los modos de apertura

    // Prepara una cadena para mostrar la información del duplicado
    if (snprintf(aux, MAXNAME, "duplicado de %d (%s)", df, name) >= 0) {
        item = defineItem(df,flags,aux);
        if(insertItemF(item,openFileList))                  // Añade el nuevo descriptor duplicado a la lista de archivos abiertos
            printf("Archivo duplicado: Descriptor: %d -> %s %s\n", df, name, getFileMode(flags));
        else
            fprintf(stderr,"No se ha podido duplicar %s",name);
    }
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
    //Si se pasa un argumento con el comando help
    if (pieces[1] != NULL) {
        if (strcmp(pieces[1],"-all") == 0) {
            printCommandDescriptionListC(*commandList);      //Muestra la descripción de todos los comandos
            return;
        }
        //Busca la posición del comando en la lista de comandos
        int pos = FindCommandC(commandList,pieces[1]);
        if(pos != CNULL) {                               //Si encuentra el comando, lo imprime
            tCommandC command= getCommandC(pos,*commandList);
            printf("%s %s\n", command.name,command.description);
            return;
        }
        //Si el comando no es válido, imprime un error
        fprintf(stderr, "Comando %s no encontrado, \"help\" para ver los disponibles\n", pieces[1]);  // fprintf escribe una cadena de caracteres formateada en un archivo o flujo determinado, en este caso stderr, como es un flujo no bufferizado los errores se muestran de inmediato en la salida de errores
    }else{
        //Si no se pasa ningún argumento, imprime solo la lista de comandos disponibles
        printCommandListC(*commandList);    //Muestra los comandos sin sus descripciones
    }
}

//Comando que limpia todas las listas, cierra los archivos y finaliza el programa
void command_exit(bool *finished,OpenFileList *openFileList, HistoryList *history, CommandListC *commandList, MemoryBlockList *memoryBlockList, DirectoryList *directoryList, ProcessList *processList) {

    // Limpiamos las listas utilizadas en el programa
    CleanCommandListC(commandList);     //Limpia la lista de comandos
    CleanListH(history);                //Limpia el historial de comandos
    CleanListF(openFileList);           //Limpia la lista de archivos abiertos y cierra los archivos abiertos
    cleanMemoryBlockList(memoryBlockList);  //Limpia la lista de bloques de memoria
    cleanDirectoryList(directoryList);
    CleanProcessList(processList);

    // Establece una bandera para indicar que la shell debe terminar
    *finished = true;

    // Imprime un mensaje de despedida
    printf("Saliendo de la shell...\n");
}

//Sino se le pasa argumento imprime el directorio actual, si se le pasa argumento crea un archivo con ese nomnbre, si ya existe no lo crea
void command_makefile(char *pieces[]) {
    //Si no se pasa ningún argumento, imprime el directorio de trabajo actual
    if (pieces[1] == NULL) {                                //Si no se pasa ningún argumento, imprime el directorio de trabajo actual
        char cwd[LENGTH_MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {         //Obtiene el directorio de trabajo actual
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    }else {
        //Si el archivo ya existe, imprime un mensaje de error
        if (access(pieces[1],F_OK) != -1) {                 //Verifica si el arcchivo ya existe => devuelve -1 sino 0
            fprintf(stderr,": El archivo '%s' ya existe.\n", pieces[1]);
        }else {
            FILE *logFile = fopen(pieces[1], "w");  //Crea un archivo nuevo y lo abre en modo escritura
            if (logFile != NULL && fclose(logFile) != EOF) {     //Verifica si se pudo crear y cerrar el archivo
                printf("Archivo '%s' creado correctamente.\n", pieces[1]);      //Mensaje de confirmación si se creó correctamente
            }else
                perror("Error al cerrar el archivo");    //Imprime un error si no se pudo cerrar el archivo
        }
    }

}


//Función estática (privada) que determina la letra representativa del tipo de archivo según su modo (mode_t)
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

//Función estática (privada) que obtiene los permisos de un archivo en formato de cadena de texto
static char *GetPermissions(mode_t m) {
    char *permisos;
    //Reserva memoria para almacenar los permisos (10 caracteres + espacio extra) y comprueba si la asignación fue exitosa
    if ((permisos=(char *) malloc (12))==NULL)
        return NULL;
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';     //Permiso de lectura por el propietario           /*Como esta función utiliza malloc, hace con que la función sea autónoma
    if (m&S_IWUSR) permisos[2]='w';     //Permiso de escritura por el propietario           en cuanto a la gestión de memoria, no necesita preocuparse por la asignación
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

//Crea un directorio
void command_makedir(char *pieces[]) {
    if (pieces[1] == NULL) {
        char cwd[LENGTH_MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
        return;
    }
    struct stat st = {0};
    //Verifica si eldirectorio ya existe
    if (stat(pieces[1], &st) == 0 && S_ISDIR(st.st_mode)) {
        fprintf(stderr,"Imposible crear '%s', ya existe.\n", pieces[1]);
        return;
    }
    //Crea el directorio con permisos de lectura, escritura y ejecución para el usuario
    if (mkdir(pieces[1], 0777) == -1) { //Si no tiene permisos para crear el directorio o la ruta es inválida
        perror("No se puede crear el directorio");
    } else {
        printf("Directorio %s creado\n", pieces[1]);

        //Obtener información del nuevo directorio
        if (stat(pieces[1], &st) == 0) {  //Volver a llamar a stat para obtener el modo del nuevo directorio
            // Obtener permisos del directorio
            char *permisos = GetPermissions(st.st_mode);  //Convierte el modo a formato legible
            if (permisos != NULL) {
                printf("Permisos del nuevo directorio: %s\n", permisos);
                free(permisos);  //libera la memoria
            } else {
                perror("Error al asignar memoria para los permisos:");
            }
        } else {
            perror("Error al obtener información del nuevo directorio");
        }
    }
}

//Printea el directorio actual
void command_cwd() {
    char cwd[LENGTH_MAX_PATH]; //buffer para almacenar el directorio actual

    //Obtiene el directorio actual
    if(getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("Error obteniendo el directorio actual");
    }
}


//Funcion auxiliar para obtener los datos de los archivos de un directorio si hide está activo devuelve false y se salta ese archivo
static bool GetFileData(bool hide, struct stat *data, char *dir, char *name, char *filePath) {
    if (!hide && (name[0] == '.' || strcmp(name, "..") == 0)) {              //Si 'hide' es false y el nombre del archivo comienza con '.' o es "..", se oculta
        return false;           //No se obtienen datos de archivos ocultos ni del directorio padrec
    }
    //Construye la ruta completa del archivo utilizando snprintf
    snprintf(filePath, LENGTH_MAX_INPUT, "%s/%s", dir, name);
    if (lstat(filePath, data) == -1) {           //Intenta obtener información del archivo mediante la llamada a stat
        fprintf(stderr, "Error al intentar acceder a %s: %s", name,strerror(errno));    //En caso de error, imprime un mensaje informando el nombre del archivo con problemas
        return false;
    }

    return true;        //Si se obtiene correctamente la información, retorna true
}

void print_file_info(char *name, struct stat *data, bool showLong, bool showLink, bool showAccessTime) {
    time_t t = data->st_atim.tv_sec;         //Almacenamos la hora en segundos desde 1970
    struct tm *tm_info = localtime(&t);
    char buffer[26];
    //Formatea la fecha y hora en una cadena
    strftime(buffer, 26, "%Y/%m/%d-%H:%M", tm_info);
    if (showLong) {
        char* permissions = GetPermissions(data->st_mode);       //Permisos
        //Propietario y grupo
        struct passwd *pw = getpwuid(data->st_uid);
        struct group *gr = getgrgid(data->st_gid);
        printf("%s   %ld (%ld) %s %s %s %9ld %s\n",buffer,data->st_nlink,data->st_ino,pw->pw_name,gr->gr_name,permissions,data->st_size,name);
        free(permissions);
        return;
    }if (showAccessTime) {
        printf("%ld %s %s\n",data->st_size,buffer,name);
    }if(showLink) {
        char link[LENGTH_MAX_PHRASE];
        ssize_t linklen = readlink(name,link,LENGTH_MAX_PHRASE);
        if(linklen == -1) {         //No tiene enlace simbólico
            printf("%10ld %s\n",data->st_size,name);
        }else {
            link[linklen] = '\0';  // Añadimos terminador nulo
            printf("%10ld %s -> %s\n",data->st_size,name,link);
        }
    }else
        printf("%10ld  %s\n",data->st_size,name);
}


// Unified function to list directories and files
void command_list(char *name, bool showLong, bool showLink, bool showAccessTime, bool showHidden, bool isDir) {
    char filePath[LENGTH_MAX_INPUT];       //Variable para almacenar la ruta completa del archivo (directorio + nombre del archivo)
    if (isDir) {
        DIR *dir;
        struct dirent *entry;
        // Abrir directorio
        dir = opendir(name);
        if (dir == NULL) {
            fprintf(stderr, "Error al intentar acceder a %s: %s \n", name,strerror(errno));
            return;
        }

        printf("************%s\n", name);

        // Iterar sobre los archivos del directorio
        while ((entry = readdir(dir)) != NULL) {
            struct stat data;
            if (GetFileData(showHidden, &data, name , entry->d_name,filePath)) {
                print_file_info(entry->d_name, &data, showLong,showLink,showAccessTime);  // Usar la función auxiliar
            }
        }
        closedir(dir);
    } else {
        struct stat data;
        if (lstat(name, &data) == -1) {
            fprintf(stderr, "Error al intentar acceder a %s: %s \n", name,strerror(errno));
            return;;
        }
        print_file_info(name, &data,showLong,showLink,showAccessTime);  // Usar la función auxiliar
    }
}


//Intepreta los argumentos y llama a listDirectoryRecursively cpn los parametros adecuados
void command_list_File_Dir(char *pieces[]) {
    bool showHidden = false;
    bool showLong = false; //si se debe mostrar información detallada
    bool showLink = false; //si se deben mostrar los enlaces simbólicos
    bool showAccessTime = false; //si se deben mostrar las fechas de acceso
    bool isDir = (strcmp(pieces[0], "listfile") == 0) ? false : true;
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

    //Si no se especifican directorios imprimimos el directorio actual
    if (pieces[i] == NULL) {
        char cwd[LENGTH_MAX_INPUT];
        if (getcwd(cwd, LENGTH_MAX_INPUT) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("Error obteniendo el directorio actual");
        }
    }
    //Listar directorios recursivamente
    while (pieces[i] != NULL) {
        command_list(pieces[i], showLong, showLink, showAccessTime,showHidden,isDir);
        i++;
    }
}

static void listDirectoryRecursively(char *dirName, bool showHidden, bool showLong, bool showLink, bool showAccessTime) {
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
    bool isLink = !showLink && lstat(dirName, &fileStat) == 0 && S_ISLNK(fileStat.st_mode);
    if (!isLink) {
        printf("************%s\n", dirName);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (GetFileData(showHidden, &fileStat, dirName , entry->d_name,fullPath)) {
            print_file_info(entry->d_name, &fileStat, showLong,showLink,showAccessTime);  // Usar la función auxiliar
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
            fprintf(stderr, "Opción %s no reconocida: %s\n", pieces[i],strerror(errno));
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


static void listDirectoryRecursivelyReverse(char *dirName, bool showHidden, bool showLong, bool showLink, bool showAccessTime){
    DIR *dir;
    struct dirent *entry;
    //se abre el directorio especidifcado por dirName
    dir = opendir(dirName);
    if (dir == NULL) {
        fprintf(stderr, "Error al intentar acceder a %s: %s\n", dirName,strerror(errno));  // Mensaje de error
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
            fprintf(stderr, "Error al intentar acceder a %s: %s\n", entry->d_name,strerror(errno));  // Mensaje de error
            continue;
        }
        if (S_ISDIR(fileData.st_mode)) {
            listDirectoryRecursivelyReverse(fullPath, showHidden, showLong, showLink, showAccessTime);
        }
    }

    // Ahora que hemos recorrido recursivamente, volvemos a listar el contenido del directorio actual
    rewinddir(dir);  // Volver al inicio del directorio para la segunda pasada (evitar cerrar/abrir)

    bool isLink = !showLink && lstat(dirName, &fileData) == 0 && S_ISLNK(fileData.st_mode);
    if (!isLink) {
        printf("************%s\n", dirName);
    }



    while ((entry = readdir(dir)) != NULL) {
        if (GetFileData(showHidden, &fileData, dirName , entry->d_name,fullPath)) {
            print_file_info(entry->d_name, &fileData, showLong,showLink,showAccessTime);  // Usar la función auxiliar
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
                fprintf(stderr, "Opción %s no reconocida: %s", pieces[i],strerror(errno));
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
    struct stat st;         //Estructura para almacenar información sobre el archivo o directorio

    //Si stat falla (retorna -1), imprime un mensaje de error y retorna 'false'
    if (stat(path, &st) == -1) {
        //Imprimir un mensaje de error detallado en stderr indicando que hubo un problema con 'path'
        fprintf(stderr, "Error al intentar acceder a %s: %s\n", path, strerror(errno));
        return false;
    }
    //Retorna 'true' si el 'st_mode' indica que la ruta es un directorio, de lo contrario, 'false'
    return S_ISDIR(st.st_mode);
}

//delrec [name1 name2 ..]	Borra ficheros o directorios no vacios recursivamente

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
            fprintf(stderr, "Error al eliminar directorio %s: %s\n", fullPath, strerror(errno));
            return false;
        }
    } else {
        // Si es un archivo regular o un enlace simbólico, lo eliminamos
        if (unlink(fullPath) == -1) {
            //Error al eliminar el archivo
            fprintf(stderr, "Error al eliminar el archivo %s: %s\n", name, strerror(errno));
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
            fprintf(stderr, "Error al obtener el directorio actual: %s\n", strerror(errno));
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

//función asigna memoria compartida
static void * ObtenerMemoriaShmget (key_t clave, size_t tam){
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL; /*cuando no es crear pasamos de tamano 0*/
    if (clave==IPC_PRIVATE)  /*no nos vale*/
        {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
             shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s); /* si no es crear, necesitamos el tamano, que es s.shm_segsz*/
 /* Guardar en la lista   InsertarNodoShared (&L, p, s.shm_segsz, clave); */
    return (p);
}


//función gestiona la asignación de memoria compartida
static void do_AllocateCreateshared(char *pieces[], MemoryBlockList *memoryBlockList) {
    if (pieces[0] == NULL) {
        // Caso 1: "allocate -createshared"
        printEspecificBlocks(*memoryBlockList, SHARED_MEMORY);
        return;
    }

    key_t cl = (key_t)strtoul(pieces[0], NULL, 10); // Convertir clave

    if (pieces[1] == NULL) {
        printEspecificBlocks(*memoryBlockList, SHARED_MEMORY);
        return;
    }

    size_t tam = (size_t)strtoul(pieces[1], NULL, 10); // Convertir tamaño

    if (tam == 0) {
        fprintf(stderr, "No se asignan bloques de 0 bytes\n");
        return;
    }

    // Caso 3: "allocate -createshared <key> <size>"
    void *p = ObtenerMemoriaShmget(cl, tam);
    if (p != NULL) {
        printf("Asignados %lu bytes en %p\n", (unsigned long)tam, p);

        if (!insertMemoryBlockB(memoryBlockList, p, tam, SHARED_MEMORY, cl, BNULL, -1)) {
            fprintf(stderr, "Error al insertar el bloque de memoria compartida en la lista\n");
        }
    } else {
        fprintf(stderr, "Imposible asignar memoria compartida clave %lu: %s\n",
                (unsigned long)cl, strerror(errno));
    }
}

//función intenta asignar un segmento de memoria compartida existente
static void do_AllocateShared(char *pieces[], MemoryBlockList *memoryBlockList) {
    key_t cl;
    void *p;

    if (pieces[0] == NULL) {
        printEspecificBlocks(*memoryBlockList, SHARED_MEMORY);
        return;
    }

    cl = (key_t) strtoul(pieces[0], NULL, 10);

    if ((p = ObtenerMemoriaShmget(cl, 0)) != NULL) {
        printf("Memoria compartida de clave %lu en %p\n", (unsigned long)cl, p);
        struct shmid_ds s;
        if (shmctl(shmget(cl, 0, 0666), IPC_STAT, &s) == 0) {
            if (!insertMemoryBlockB(memoryBlockList, p, s.shm_segsz, SHARED_MEMORY, cl, NULL, -1)) {
                fprintf(stderr, "Error al insertar el bloque de memoria compartida en la lista\n");
                shmdt(p);
            }
        }
    } else {
        fprintf(stderr, "Imposible asignar memoria compartida clave %lu: %s\n", (unsigned long)cl, strerror(errno));
    }
}

//función mapea un archivo en la memoria del proceso usando mmap
static void * MapearFichero (char * fichero, int protection, OpenFileList *openFileList, MemoryBlockList *memoryBlockList){
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;

    // Añade el df a la lista de ficheros abiertos
    tItemF item = defineItem(df, modo, fichero);
    if (!insertItemF(item, openFileList) || !insertMemoryBlockB(memoryBlockList, p, s.st_size, MAPPED_FILE, -1, fichero, df)) {
        perror("Error al insertar el descriptor o el bloque en las listas");
        munmap(p, s.st_size);
        close(df);
        return NULL;
    }


    return p;
}
//Función llama a MapearFichero para mapear un archivo
static void do_AllocateMmap(char *pieces[], MemoryBlockList *memoryBlockList, OpenFileList *openFileList)
{
    char *perm;
    void *p;
    int protection=0;

    if (pieces[0]==NULL)
    {printEspecificBlocks(*memoryBlockList,MAPPED_FILE); return;}
    if ((perm=pieces[1])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }

    // Mapear el archivo
    if ((p = MapearFichero(pieces[0], protection, openFileList,memoryBlockList)) == NULL) {
        perror("Imposible mapear fichero");
    } else {
        printf("Fichero %s mapeado en %p\n", pieces[0], p);
    }
}


//Es un gestor de asignación de memoria que recibe argumentos y decide qué tipo de asignación realizar
void command_allocate(char *pieces[], MemoryBlockList *memoryBlockList, OpenFileList *openFileList) {
    if (pieces[1] == NULL) {
        printAllBlocks(*memoryBlockList);
        return;
    }

    if (strcmp(pieces[1], "-malloc") == 0) {
        if (pieces[2] == NULL) {
            printEspecificBlocks(*memoryBlockList, MALLOC_MEMORY);
            return;
        }
        size_t n = (size_t) strtoul(pieces[2], NULL, 10);
        if (n == 0) {
            fprintf(stderr, "No se asignan bloques de 0 bytes\n");
            return;
        }
        void *address = malloc(n);
        if (address == NULL) {
            perror("Error al asignar memoria con malloc");
            return;
        }

        if (!insertMemoryBlockB(memoryBlockList, address, n, MALLOC_MEMORY, -1, BNULL, -1)) {  //inserta en la lista
            fprintf(stderr, "Error al insertar el bloque malloc en la lista de memoria\n");
            free(address);
        } else {
            printf("Asignados %zu bytes en %p\n", n, address);
        }

    } else if (strcmp(pieces[1], "-mmap") == 0) {
        if (pieces[2] == NULL) {
            printEspecificBlocks(*memoryBlockList, MAPPED_FILE);
        } else {
            do_AllocateMmap(pieces + 2, memoryBlockList, openFileList);
        }

    } else if (strcmp(pieces[1], "-createshared") == 0) {
        if (pieces[2] == NULL) {
            printEspecificBlocks(*memoryBlockList, SHARED_MEMORY);
        } else {
            do_AllocateCreateshared(pieces + 2, memoryBlockList);
        }

    } else if (strcmp(pieces[1], "-shared") == 0) {
        if (pieces[2] == NULL) {
            printEspecificBlocks(*memoryBlockList, SHARED_MEMORY);
        } else {
            do_AllocateShared(pieces + 2, memoryBlockList);
        }
    } else {
        fprintf(stderr, "Opción desconocida para allocate\n");
    }
}


static void do_DeallocateMalloc(char *args[], MemoryBlockList *memblocks) {
    char *NoValidCharacter;
    if (args[0] != NULL) {
        size_t n = strtoul(args[0], &NoValidCharacter, 10); // Convertir argumento a número
        if (*NoValidCharacter == '\0') { // Validar que el número es válido
            if (n > 0) {
                tPosB p = firstB(*memblocks);
                while (p != BNULL) {
                    if (p->data.type == MALLOC_MEMORY && p->data.size == n) {
                        break; // Encontramos el bloque
                    }
                    p = nextB(p);
                }
                if(p == BNULL) {
                    printf("No hay bloque de ese tamaño asignado con malloc\n");
                }else {
                    free(p->data.address);
                    removeMemoryBlock(memblocks,p);
                }
            }else {
                printf("No hay bloque de ese tamaño asignado con malloc\n");
            }
        } else if (NoValidCharacter == args[0]) { // No se pudo convertir nada
            printEspecificBlocks(*memblocks,MALLOC_MEMORY);
        } else {
            perror("No hay bloque de ese tamaño asignado con malloc\n");
        }
    } else {
        printEspecificBlocks(*memblocks,MALLOC_MEMORY);
    }
}

static void do_DeallocateMmap (char *args[],MemoryBlockList *memblocks) {
    if (args[0] != NULL) {
        tPosB p = firstB(*memblocks);
        while (p != BNULL) {
            if (p->data.type == MAPPED_FILE && strcmp(args[0],p->data.fileName) == 0){
                break; // Encontramos el bloque
            }
            p = nextB(p);
        }

        // Verificar si se encontró el bloque, sino imprimimos error y salimos para no pasarle un puntero nulo a munmap
        if (p == BNULL) {
            fprintf(stderr,"Fichero %s no mapeado\n", args[0]);
            return;
        }

        if(munmap(p->data.address,p->data.size) == -1) {
            perror("Error al desmapear el archivo");
        }
        free(p->data.fileName);
        removeMemoryBlock(memblocks,p);
    }else {
        printEspecificBlocks(*memblocks,MAPPED_FILE);
    }
}

static void do_DeallocateShared (char *args[], MemoryBlockList *memblocks) {
    if (args[0] == NULL) {
        printEspecificBlocks(*memblocks, SHARED_MEMORY);
        return;
    }
    key_t clave;
    if ((clave=(key_t) strtoul(args[0],NULL,10))==IPC_PRIVATE){
        printf ("shared necesita clave_valida\n");
        return;
    }
    //Buscar el bloque correspondiente de la lista
    tPosB p = firstB(*memblocks);
    while (p != BNULL) {
        if (p->data.type == SHARED_MEMORY && p->data.smKey == clave) {
            break; // Encontramos el bloque
        }
        p = nextB(p);
    }
    if (p == BNULL) {
        printf("No hay bloque de memoria compartida con la clave %d\n", clave);
        return;
    }

    // Intentar desvincular la memoria compartida
    if (shmdt(p->data.address) == -1) {
        perror("Error al desvincular memoria compartida");
        return;
    }

    removeMemoryBlock(memblocks, p);
}



//función elimina un segmento de memoria compartida según su clave
static void do_DeallocateDelkey (char *args[]){
    key_t clave;
    int id;
    char *key=args[0];

    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("delkey necesita clave_valida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n");
}

//Convierte la cadena a un número hexadecimal
void *cadtop(char *cadena) {
    //strtoull de la biblioteca estándar de C para convertir la cadena hexadecimal a un número sin signo de 64 bits
    //(unsigned long long int).
    unsigned long long int direccion = strtoull(cadena, NULL, 16); //16 indica la base hexadecimal.
    //Luego, ese número se interpreta como una dirección de memoria y se convierte en un puntero.
    return (void *)direccion;
}

static void do_DeallocateAdd (char *args[], MemoryBlockList *memblocks) {
    void *addr = cadtop(args[0]);  // Convertir la dirección de memoria de cadena a puntero
    tPosB p = firstB(*memblocks);

    while (p != BNULL) {
        if (p->data.address == addr) {
            break; // Encontramos el bloque
        }
        p = nextB(p);
    }
    if (p == BNULL) {
        fprintf(stderr,"Direccion %p no asignada con malloc, shared o mmap\n",args[0]);
    }else {
        switch (p->data.type) {
            case MALLOC_MEMORY:
                free(addr);
                break;
            case SHARED_MEMORY:
                if (shmdt(p->data.address) == -1)
                    perror("Error al desvincular memoria compartida");
                break;
            case MAPPED_FILE: // Identificador es un nombre de archivo
                if(munmap(p->data.address,p->data.size) == -1) {
                    perror("Error al desmapear el archivo");
                }
                free(p->data.fileName);
                break;
            default: // Identificador es una dirección (void *)
                perror("Tipo no reconocido");
                break;
        }
        removeMemoryBlock(memblocks,p);
    }
}

void command_deallocate(char *pieces[],MemoryBlockList *memblocks) {
    if (pieces[1] != NULL) {
        if (strcmp(pieces[1], "-malloc") == 0) {
            do_DeallocateMalloc(pieces + 2, memblocks);
        }else if (strcmp(pieces[1], "-mmap") == 0) {
            do_DeallocateMmap(pieces + 2,memblocks);
        }else if (strcmp(pieces[1], "-shared") == 0) {
            do_DeallocateShared(pieces + 2,memblocks);
        }else if (strcmp(pieces[1], "-delkey") == 0) {
            do_DeallocateDelkey(pieces + 2);
        }else {
            do_DeallocateAdd(pieces + 1, memblocks);
        }
    }else
        printAllBlocks(*memblocks);
}


//Llena una región de memoria con un valor específico
void LlenarMemoria (void *p, size_t cont, unsigned char byte){
    unsigned char *arr=(unsigned char *) p;
    size_t i;

    for (i=0; i<cont;i++)
        arr[i]=byte;
}

//Llena una región de memoria con un byte específico
void command_memfill(char *pieces[]) {
    if (pieces[1] == NULL) {
        fprintf(stderr, "Error: Faltan argumentos para el comando memfill\n");
        fprintf(stderr, "Uso: memfill <addr> <cont> <ch>\n");
        return;
    }

    void *addr = cadtop(pieces[1]);  // Convertir la dirección de memoria de cadena a puntero
    size_t cont = 128;  // Valor por defecto 128 si no se pasa un segundo argumento
    unsigned char ch = 0x41;  // Valor por defecto 'A' (0x41) si no se pasa un tercer argumento
    if (pieces[2] != NULL) {
        cont = (size_t) strtoul(pieces[2], NULL, 10);  // Convertir el tamaño de la memoria
    }

    if (pieces[3] != NULL) {
        if (pieces[3][0] == '0' && pieces[3][1] == 'x') {    // Verificar si el tercer argumento es un número hexadecimal
            ch = (unsigned char) strtoul(pieces[3], NULL, 16);   // Convertir el número hexadecimal a byte
        } else if (isdigit(pieces[3][0])) {  // Verificar si el tercer argumento es un número decimal
            ch = (unsigned char) strtoul(pieces[3], NULL, 10);   // Convertir el número decimal a byte
        } else if (pieces[3][0] == '\'' && pieces[3][1] != '\0' && pieces[3][2] == '\'' && pieces[3][3] == '\0') {
            ch = pieces[3][1];   // Convertir el carácter entre comillas
        } else {
            fprintf(stderr, "Error: Formato de byte no válido (debe ser un número o un carácter entre comillas)\n");
            return;
        }
    }

    LlenarMemoria(addr, cont, ch);  // Llenar la memoria con el carácter 'ch' para los 'cont' bytes
    printf("Llenando %zu bytes de memoria con el byte %c(0x%x) a partir de la direccion %p\n", cont, ch, ch, addr);
}
void command_memdump(char *pieces[]) {
    if (pieces[1] != NULL) {
        void *addr = cadtop(pieces[1]);  // Convertir la dirección de memoria desde la que se empiezan a leer los bytes de cadena a puntero
        unsigned char *byte_ptr = (unsigned char *) addr;
        size_t cont, i, j;

        if (pieces[2] != NULL) {
            cont = (size_t) strtoul(pieces[2], NULL, 10);  // Convertir el número de bytes a leer de char a long unsigned int
        }else {
            cont = 25;  //Tamaño a volcar predeterminado (25 bytes)
        }
        printf("Volcando %lu bytes desde la direccion %p\n",cont,addr);
        // Volcamos los contenidos de memoria en bloques de 16 bytes
        for(i = 0; i < cont; i+=25) {
            printf(" ");  // Espaciado inicial para alineación

            // Imprimir los caracteres imprimibles correspondientes a los bytes
            for (j = 0; j < 25 && i + j < cont; ++j) {
                unsigned char byte = byte_ptr[i + j];
                // Si el byte es imprimible, lo mostramos; de lo contrario, mostramos '.'
                printf(" %c ", isprint(byte) ? byte : '.');
            }
            printf("\n");
            printf(" ");
            for(j = 0; j < 25 && i + j < cont; j++) {
                printf("%02x ", byte_ptr[i + j]);
            }
            printf("\n");
        }
    }
}

//Ejecuta un comando equivalente a pmap para ver el mapa de memoria del proceso actual
void Do_pmap (void){    /*sin argumentos*/
    pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *pieces[4]={"pmap",elpid,NULL};

    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
    }
    if (pid==0){
        if (execvp(pieces[0],pieces)==-1)
            perror("cannot execute pmap (linux, solaris)");

        pieces[0]="procstat"; pieces[1]="vm"; pieces[2]=elpid; pieces[3]=NULL;
        if (execvp(pieces[0],pieces)==-1)/*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        pieces[0]="procmap",pieces[1]=elpid;pieces[2]=NULL;
        if (execvp(pieces[0],pieces)==-1)  /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        pieces[0]="vmmap"; pieces[1]="-interleave"; pieces[2]=elpid;pieces[3]=NULL;
        if (execvp(pieces[0],pieces)==-1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");
        exit(1);
    }
    waitpid(pid,NULL,0);
 }


//Print direcciones
void command_memory(char *pieces[], MemoryBlockList memoryBlockList) {
    if (pieces[1] == NULL) {
        fprintf(stderr, "Error: Faltan argumentos para el comando memory\n");
        return;
    }

    if (strcmp(pieces[1], "-funcs") == 0) {

        printf("Funciones programa      %p,    %p,    %p\n", (void *)command_memory, (void *)command_allocate, (void *)command_deallocate);

        printf("Funciones libreria      %p,    %p,    %p\n", (void *)printf, (void *)malloc, (void *)free);

    } else if (strcmp(pieces[1], "-vars") == 0) {

        int local_var1, local_var2, local_var3;
        printf("Variables locales       %p,    %p,    %p\n", (void *)&local_var1, (void *)&local_var2, (void *)&local_var3);

        printf("Variables globales      %p,    %p,    %p\n", (void *)&ext_init_var1, (void *)&ext_init_var2, (void *)&ext_init_var3);

        printf("Var (N.I.)globales      %p,    %p,    %p\n", (void *)&ext_var1, (void *)&ext_var2, (void *)&ext_var3);

        static int static_var1, static_var2, static_var3;
        printf("Variables staticas      %p,    %p,    %p\n", (void *)&static_var1, (void *)&static_var2, (void *)&static_var3);

        static int static_init_var1, static_init_var2, static_init_var3;
        printf("Var (N.I.)staticas      %p,    %p,    %p\n", (void *)&static_init_var1, (void *)&static_init_var2, (void *)&static_init_var3);


    } else if (strcmp(pieces[1], "-blocks") == 0) {
        printAllBlocks(memoryBlockList);

    } else if (strcmp(pieces[1], "-all") == 0) {
        //Mostrar todo (funciones, variables, bloques)
        command_memory((char *[]){"memory", "-funcs", NULL},memoryBlockList);
        command_memory((char *[]){"memory", "-vars", NULL},memoryBlockList);
        command_memory((char *[]){"memory", "-blocks", NULL},memoryBlockList);


    } else if (strcmp(pieces[1], "-pmap") == 0) {
        Do_pmap();
    } else {
        fprintf(stderr, "Opción desconocida para memory\n");
    }
}

//Lee un archivo en una región de memoria específica
ssize_t LeerFichero (char *f, void *p, ssize_t cont)        //ssize_t es long int
{
    struct stat s;
    ssize_t  n;
    int df,aux;

    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    if ((n=read(df,p,cont))==-1){
        aux=errno;      //Se guarda el valor actual de la salida de erro
        close(df);      //Se cierra el df obviando, si se obtiene, el código de error
        errno=aux;      //Se vuelve a establecer el error que habíamos obtenido
        return -1;      //Se devuelve -1 porque no hemos podido leer el fichero
    }
    close (df);
    return n;
}

void command_readfile(char *ar[]) {
    void *p;
    ssize_t cont=-1;  /*si no pasamos tamano se lee entero */
    ssize_t n;
    if (ar[1]==NULL || ar[2]==NULL){
        printf ("faltan parametros\n");
        return;
    }

    // ar[1]: Ruta del archivo
    // ar[2]: Dirección de memoria (convertida a puntero)
    p=cadtop(ar[2]);  /*convertimos de cadena a puntero*/

    // Si se proporciona ar[3], lo interpretamos como el tamaño a leer
    if (ar[3]!=NULL)
        cont= atoll(ar[3]);

    if ((n=LeerFichero(ar[1],p,cont))==-1)
        perror ("Imposible leer fichero");
    else
        printf ("leidos %lld bytes de %s en %p\n",(long long) n,ar[1],p);
}

//Escribe en un archivo desde una dirección de memoria específica
void command_writefile(char *pieces[]) {
    char *filename;
    void *addr;
    size_t cont;
    ssize_t written;
    int fd;

    // Verificar que los argumentos mínimos están presentes
    if (pieces[1] == NULL || pieces[2] == NULL || pieces[3] == NULL) {
        fprintf(stderr, "Error: Faltan argumentos para el comando writefile\n");
        fprintf(stderr, "Uso: writefile [-o] <filename> <addr> <cont>\n");
        return;
    }

    // Verificar si el primer argumento es "-o" para sobrescribir
    int overwrite = 0;
    int base_index = 1;
    if (strcmp(pieces[1], "-o") == 0) {
        overwrite = 1;
        base_index++;
    }

    // Asignar los argumentos a las variables correspondientes
    filename = pieces[base_index];
    addr = (void *)strtoull(pieces[base_index + 1], NULL, 16);
    cont = (size_t)strtoull(pieces[base_index + 2], NULL, 10);

    // Verificar si el archivo ya existe y manejar según el modo
    if (!overwrite && access(filename, F_OK) != -1) {
        fprintf(stderr, "Imposible escribir fichero: File exists\n");
        return;
    }

    // Abrir archivo con los flags correctos según el modo de sobrescritura
    int flags = O_WRONLY | O_CREAT | (overwrite ? O_TRUNC : O_EXCL);
    fd = open(filename, flags, 0666);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        return;
    }

    // Intentar escribir en el archivo desde `addr`
    written = write(fd, addr, cont);
    if (written == -1) {
        perror("Error al escribir en el archivo");
        close(fd);
        return;
    } else if ((size_t)written != cont) {
        fprintf(stderr, "Advertencia: No se pudieron escribir todos los bytes\n");
    }

    printf("Escritos %zd bytes en %s desde la dirección de memoria %p\n", written, filename, addr);

    close(fd);
}

//Lee un archivo en una región de memoria específica
ssize_t LeerDF (int df, void *p, ssize_t cont)
{
    struct stat s;
    ssize_t  n;
    // Obtener información del archivo si es necesario, es decir si queremos leerlo todo simplemente obtenemos el tamaño para leerlo todo
    if (cont == -1) {
        if (fstat(df, &s) == -1) {
            perror("Error obteniendo información del archivo");
            return -1;
        }
        cont = s.st_size;
    }

    if ((n=read(df,p,cont))==-1)
        return -1;      //Se devuelve -1 porque no hemos podido leer el fichero

    return n;
}

void command_read(char *ar[], OpenFileList *file_list) {
    void *p;
    ssize_t cont=-1;  /*si no pasamos tamano se lee entero */
    ssize_t n;
    int fd;

    if (ar[1]==NULL || ar[2]==NULL){
        printf ("faltan parametros\n");
        return;
    }

    fd = (int)strtol(ar[1], NULL, 16);
    // ar[1]: Ruta del archivo
    // ar[2]: Dirección de memoria (convertida a puntero)
    p = cadtop(ar[2]);  /*convertimos de cadena a puntero*/

    if (findFile(fd,*file_list) == NULL) {
        perror("fd no válido");
        return;
    }

    // Si se proporciona ar[3], lo interpretamos como el tamaño a leer
    if (ar[3]!=NULL) {
        cont= atoll(ar[3]);
        if (cont == 0) {
            printf("Error: El tamaño a leer no puede ser 0\n");
            return;
        }
    }

    if ((n=LeerDF(fd,p,cont))==-1)
        perror ("Imposible leer fichero");
    else
        printf ("leidos %lld bytes del descriptor %s en la %p\n",(long long) n,ar[1],p);
}


void command_write(char *pieces[]) {
    int fd;
    void *addr;
    size_t cont;
    ssize_t written;

    // Verificar que los argumentos mínimos están presentes
    if (pieces[1] == NULL || pieces[2] == NULL || pieces[3] == NULL) {
        fprintf(stderr, "Error: Faltan argumentos para el comando write\n");
        fprintf(stderr, "Uso: write <fd> <addr> <cont>\n");
        return;
    }

    // Convertir los argumentos
    fd = (int) strtol(pieces[1], NULL, 10);
    if (fd < 0) {
        fprintf(stderr, "Error: Descriptor de archivo no válido\n");
        return;
    }
    addr = cadtop(pieces[2]);
    if (addr == NULL) {
        fprintf(stderr, "Error: Dirección de memoria no válida\n");
        return;
    }
    cont = (size_t) strtoul(pieces[3], NULL, 10);
    if (cont == 0) {
        fprintf(stderr, "Error: Tamaño de contenido no válido\n");
        return;
    }

    // Escribir en el archivo desde la dirección de memoria especificada
    written = write(fd, addr, cont);
    if (written == -1) {
        perror("Error al escribir en el archivo");
    } else {
        printf("Escritos %zd bytes en el descriptor %d desde la direccion %p\n", written, fd, addr);
    }
}


//Función recursiva que muestra la dirección de su parámetro
void Recursiva (int n){
        char automatico[TAMANO];          // Array automático (en la pila)
    static char estatico[TAMANO];     // Array estático (en memoria estática)
    /* Imprime:
     * 1º: Valor del parámetro actual (`n`) de la llamada
     * 2º: Dirección de la variable `n` (almacenada en la pila, cambia en cada llamada)
     * 3º: Dirección del array `automatico` (local a cada llamada, por lo que cambia en cada iteración)
     * 4º: Dirección del array `estatico` (compartida entre todas las llamadas, no cambia)
     */
    printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

    if (n>0)
        Recursiva(n - 1); // Llamada recursiva: reduce el valor de `n`
        // Cada llamada agrega un nuevo marco a la pila, modificando las direcciones locales
}

void command_recurse(char *pieces[]) {
    if (pieces[1] != NULL) {
        int n = atoi(pieces[1]);
        Recursiva(n);
    }
}


void command_getuid() {
    uid_t real_uid = getuid();   // UID real
    uid_t effective_uid = geteuid(); // UID efectivo

    // Obtener el nombre del usuario a partir de los UID
    struct passwd *real_pw = getpwuid(real_uid);
    struct passwd *effective_pw = getpwuid(effective_uid);

    // Mostrar credenciales reales
    if (real_pw) {
        printf("Credencial real: %d, (%s)\n", real_uid, real_pw->pw_name);
    } else {
        printf("Credencial real: %d, (desconocido)\n", real_uid);
    }

    // Mostrar credenciales efectivas
    if (effective_pw) {
        printf("Credencial efectiva: %d, (%s)\n", effective_uid, effective_pw->pw_name);
    } else {
        printf("Credencial efectiva: %d, (desconocido)\n", effective_uid);
    }
}

static long int getID(char *id) {
    char *NoValidCharacter;
    const long int n = strtol(id,&NoValidCharacter,10);     //Variable para almacenar el número si lo hay

    if(NoValidCharacter == id) {              //Si todo el argumento es un número válido, entonces el puntero NoValidCharacter apuntará al \0, strtol convirtió la cadena exitosamente
        perror("No se introdujo ningún número válido");                    //Si no se introdujo ningún número válido el puntero apuntará a la primera posición de pieces[1],
        return -1;
    }
    if (*NoValidCharacter != '\0') {
        fprintf(stderr,"Parte de la cadena no es válida: %s\n", NoValidCharacter);      //Se han introducido carácteres válidos antes de que el puntero apunte a uno no válido
        printf("Parte numérica leída: %li\n", n);
    }
    return n;
}

//Credencial real -> es la del usuario que ejecuta el programa
//Credencial efectiva -> es la del propietario del archivo ejecutable (p3)
/*
 *  Cuando un programa con el bit SUID (rwsr-xr-x) es ejecutado,
 *  el UID efectivo se establece al propietario del archivo,
 *  mientras que el UID real permanece igual.
 */
void command_setuid(char *pieces[]) {
    if (pieces[1] != NULL) {
        if (strcmp(pieces[1], "-l") == 0) {
            if (pieces[2] != NULL) {
                // Opción -l: cambiar credencial usando un nombre de usuario
                struct passwd *pw = getpwnam(pieces[2]);
                if (pw == NULL) {
                    fprintf(stderr,"Usuario %s no encontrado\n", pieces[2]);      //Se han introducido carácteres válidos antes de que el puntero apunte a uno no válido
                    return;
                }
                if (setuid(pw->pw_uid) == 0) {
                    printf("UID cambiado exitosamente a %d (%s)\n", pw->pw_uid, pieces[2]);
                }else {
                    perror("Error al cambiar UID");
                }
            }else
                command_getuid();
        }else{
            long int uid = getID(pieces[1]);
            if (uid == -1) {
                fprintf(stderr, "UID inválido: %s\n", pieces[1]);
                return;
            }
            if (setuid((uid_t)uid) == 0) {
                printf("UID cambiado exitosamente a %ld\n", uid);
            } else {
                perror("Error al cambiar UID");
            }
        }
    }else {
        command_getuid();
    }
}



int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parámetro*/
{                                           /*devuelve la posicion de la variable en el entorno, -1 si no existe*/
    int pos=0;
    char aux[LENGTH_MAX_PHRASE];

    strcpy (aux,var);
    strcat (aux,"=");

    while (e[pos]!=NULL)
        if (!strncmp(e[pos],aux,strlen(aux)))       //Si los n (strlen(aux)) primeros bytes de las cadenas son iguales devuelve pos
            return (pos);
        else
            pos++;
    errno=ENOENT;   /*no hay tal variable*/
    return(-1);
}

int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parÃ¡metro*/
{                                                        /*lo hace directamente, no usa putenv*/
    int pos;
    char *aux;

    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);

    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;
    strcpy(aux,var);
    strcat(aux,"=");
    strcat(aux,valor);
    e[pos]=aux;
    return (pos);
}

extern char **environ;         // Use the global environ variable

//mostra el valor y la direccion de las variables de entorno especificadas por el usuario
void command_showvar(char *pieces[], char *env[]) {
    if (pieces[1] == NULL) {
        // Mostrar todas las variables de entorno
        for (int i = 0; env[i] != NULL; i++) {
            printf("%p->main arg3[3][%d]=(%p) %s\n", (void *)&env[i], i, (void *)env[i], env[i]);
        }
    } else {
        int pos;
        if ((pos = BuscarVariable(pieces[1], env)) != -1) {
            char *value = getenv(pieces[1]);
            printf("Con arg3 main %s=%s(%p) @%p\n", pieces[1], value, (void *)env, (void *)&env[pos]);
            printf("  Con environ %s=%s(%p) @%p\n", pieces[1], value, (void *)environ, (void *)environ[pos]);
            printf("   Con getenv %s(%p)\n", value, (void *)value);
        } else if ((pos = BuscarVariable(pieces[1], environ)) != -1) {
            // Caso en el que la variable se encuentra en "environ"
            char *value = getenv(pieces[1]);
            printf("Con environ %s=%s(%p) @%p\n", pieces[1], value, (void *)value, (void *)&environ[pos]);
            printf("   Con getenv %s(%p)\n", value, (void *)value);
        } else {
            // Caso en el que la variable no se encuentra
            printf("Variable: %s no encontrada en el entorno\n", pieces[1]);
        }
    }
}

void command_changevar(char *pieces[], char *env[]) {
    if (pieces[1] == NULL || pieces[2] == NULL || pieces[3] == NULL) {
        fprintf(stderr, "Uso: changevar [-a|-e|-p] var valor\n");
        return;
    }

    if (strcmp(pieces[1], "-a") == 0) {
        int result = CambiarVariable(pieces[2], pieces[3], env);
        if (result == -1) {
            perror("Error");
        }
    } else if (strcmp(pieces[1], "-e") == 0) {
        int result = CambiarVariable(pieces[2], pieces[3], environ);
        if (result == -1) {
            perror("Error");
        }
    } else if (strcmp(pieces[1], "-p") == 0) {
        char *var = pieces[2];
        char *value = pieces[3];
        char *env_var = (char *)malloc(strlen(var) + strlen(value) + 2);
        if (env_var != NULL){
            sprintf(env_var, "%s=%s", var, value);
            if(putenv(env_var) != 0)
                perror("Error");
        }
    } else {
        fprintf(stderr, "Error\n");
    }
}


void command_subsvar(char *pieces[],char *env[]) {
    if (pieces[1] == NULL || pieces[2] == NULL || pieces[3] == NULL || pieces[4] == NULL) {
        fprintf(stderr, "Uso: subsvar [-a|-e] var1 var2 valor\n");
        return;
    }

    if (strcmp(pieces[1], "-a") == 0) {
        // Accede por el tercer argumento de main
        for (int i = 0; env[i] != NULL; i++) {
            if (strncmp(env[i], pieces[2], strlen(pieces[2])) == 0 && env[i][strlen(pieces[2])] == '=') {
                // Verifica si ya existe una variable con el nombre pieces[3]
                for (int j = 0; env[j] != NULL; j++) {
                    if (strncmp(env[j], pieces[3], strlen(pieces[3])) == 0 && env[j][strlen(pieces[3])] == '=') {
                        fprintf(stderr, "Imposible sustituir variable %s por %s: File exists\n", pieces[2], pieces[3]);
                        return;
                    }
                }
                // Duplica el valor actual de var1 y lo asigna a var2 en pieces[3]
                snprintf(env[i], strlen(pieces[3]) + strlen(pieces[4]) + 2, "%s=%s", pieces[3], pieces[4]);
                return;
            }
        }
    } else if (strcmp(pieces[1], "-e") == 0) {
        // Accede mediante environ usando setenv
        if (getenv(pieces[3]) != NULL) {
            fprintf(stderr, "Imposible sustituir variable %s por %s: File exists\n", pieces[2], pieces[3]);
            return;
        }
        // Elimina var1 de environ y añade var2 con el valor
        if (unsetenv(pieces[2]) == -1) {
            perror("Error al eliminar la variable de entorno");
            return;
        }
        if (setenv(pieces[3], pieces[4], 1) == -1) {
            perror("Error al establecer la nueva variable de entorno");
            return;
        }
    } else {
        fprintf(stderr, "Opcion no valida: %s\n", pieces[1]);
    }
}

void command_environ(char *pieces[], char *envp[]) {
    if (pieces[1] == NULL) {
        for (int i = 0; envp[i] != NULL; i++) {
            printf("%p->main pieces[3][%d]=(%p) %s\n", (void *)&envp[i], i, (void *)envp[i], envp[i]);
        }
        return;
    }

    if (strcmp(pieces[1], "-environ") == 0) {
        for (int i = 0; envp[i] != NULL; i++) {
            printf("%p->environ[%d]=(%p) %s\n", (void *)&envp[i], i, (void *)envp[i], envp[i]);
        }
    } else if (strcmp(pieces[1], "-addr") == 0) {
        printf("environ:   %p (almacenado en %p)\n", (void *)envp, (void *)&environ);
        printf("main pieces[3]: %p (almacenado en %p)\n", (void *)envp, (void *)&envp);
    }else {
        fprintf(stderr, "Uso: environ [-environ|-addr]\n");
    }
}
void command_fork(ProcessList *P) {
    pid_t pid;

    if ((pid=fork())==0){
        delJobs(P);
        printf ("ejecutando proceso %d\n", getpid());
    }
    else if (pid!=-1)
        waitpid (pid,NULL,0);
}

//Maybe change directoryList name to searchList.
static void getEnvariablePATH (DirectoryList *directoryList) {
    char *path = getenv("PATH");
    if (path == NULL) {
        fprintf(stderr, "No se pudo obtener la variable PATH\n");
        return;
    }

    // Hacer una copia de PATH para evitar modificarlo
    char *pathCopy = strdup(path);
    if (pathCopy == NULL) {
        perror("Error al duplicar PATH");
        return;
    }

    int directories = 0;
    char *dir = strtok(pathCopy, ":");
    while (dir != NULL) {
        if (addDirectoryD(directoryList, dir)) {
            directories++;
        }
        dir = strtok(NULL, ":");
    }
    free(pathCopy); // Liberar la memoria de la copia de PATH
    printf("Importados %d directorios en la ruta de busqueda\n", directories);
}


void command_search(char *pieces[], DirectoryList *directoryList) {
    if (pieces[1] != NULL) {
        if (strcmp(pieces[1], "-add") == 0) {
            if (pieces[2] != NULL) {
                if (!addDirectoryD(directoryList, pieces[2])) {
                    perror("Imposible insertar directorio");
                }
            }else
                perror("Imposible realizar operacion -add");
        }else if (strcmp(pieces[1], "-del") == 0){
            if (pieces[2] != NULL) {
                tPosD p = firstD(*directoryList);
                while (p != DNULL && strcmp(p->directory, pieces[2]) != 0) {
                    p = nextD(p);
                }
                if (p == DNULL) {
                    fprintf(stderr,"Direccion %s no encontrado\n",pieces[2]);
                }else {
                    removeDirectoryD(directoryList, p);
                    printf("Direccion %s eliminada\n",pieces[2]);
                }
            }
        }else if (strcmp(pieces[1], "-clear") == 0) {
            cleanDirectoryList(directoryList);
        }else if (strcmp(pieces[1], "-path") == 0) {
            getEnvariablePATH(directoryList);
        }
    }else {
        ListDirectoryList(*directoryList);
    }
}

char * Ejecutable (char *s, DirectoryList *directoryList)
{
    static char path[MAXNAME];
    struct stat st;
    char *p;

    //Si el nombre del ejecutable es nulo o la lista de rutas está vacía retornamos s.
    if (s==NULL || (p=SearchListFirstD(*directoryList))==NULL)
        return s;
    if (s[0]=='/' || !strncmp (s,"./",2) || !strncmp (s,"../",3))
        return s;        /*is an absolute pathname*/

    //Se construye una ruta completa combinando el directorio p con el nombre del archivo s
    strncpy (path, p, MAXNAME-1);strncat (path,"/",MAXNAME-1); strncat(path,s,MAXNAME-1);
    if (lstat(path,&st)!=-1)    //Si la ruta hasta el archivo existe devuelve la ruta completa
        return path;

    //Iteramos por el resto de la lista en busca de la ruta correcta para el ejecutable
    tPosD q = nextD(firstD(*directoryList));
    while (q != DNULL){
        p = GetDirectory(q);
        strncpy (path, p, MAXNAME-1);strncat (path,"/",MAXNAME-1); strncat(path,s,MAXNAME-1);
        if (lstat(path,&st)!=-1)
            return path;
        q = nextD(q);
    }
    return s;
}

int Execpve(char *tr[], char **NewEnv, long int * pprio, DirectoryList *directoryList)
{
    char *p;               /*NewEnv contains the address of the new environment*/
    /*pprio the address of the new priority*/
    /*NULL indicates no change in environment and/or priority*/
    if (tr[0]==NULL || (p=Ejecutable(tr[0], directoryList))==NULL){     //Si el nombre del ejecutable es NULL o no se encuentra ruta para el ejecutable se devuelve error
        errno=EFAULT;
        return-1;
    }
    if (pprio !=NULL  && setpriority(PRIO_PROCESS,getpid(),*pprio)==-1 && errno){
        printf ("Imposible cambiar prioridad: %s\n",strerror(errno));
        return -1;
    }

    if (NewEnv==NULL) {
        return execv(p,tr);
    }else {
        return execve (p, tr, NewEnv);
    }
}

static void liberarEnvironVars(const int *environVarsCount, char *environVars[]) {
    for (int i = 0; i < *environVarsCount; i++) {
        free(environVars[i]); // Liberar cada cadena.
    }
}

void command_exec(char *pieces[], DirectoryList *directoryList, char *env[]) {
    char *environVars[64]; // Buffer para las variables de entorno.
    int environVarsCount = 0, argumentsCount = 0;
    char *executableName = NULL;
    char *arguments[64];   // Buffer para los argumentos del ejecutable.

    // Separar variables de entorno y argumentos.
    for (int i = 0; pieces[i] != NULL; i++) {
        int envIndex = BuscarVariable(pieces[i], env);
        if (envIndex != -1) {
            // Reconstruir la variable completa: "NOMBRE=valor".
            environVars[environVarsCount] = strdup(env[envIndex]);
            environVarsCount++;
        } else {
            if (argumentsCount == 0) {
                executableName = pieces[i]; // Primer argumento no es una variable, es el ejecutable.
            }
            arguments[argumentsCount++] = pieces[i];
        }
    }

    environVars[environVarsCount] = NULL; // Terminar lista de entorno.

    // Asegurarse de que arguments[0] contenga el nombre del ejecutable.
    if (executableName == NULL) {
        fprintf(stderr, "Error: No se especificó un ejecutable.\n");
        liberarEnvironVars(&environVarsCount, environVars);
        return;
    }

    arguments[argumentsCount] = NULL; // Terminar lista de argumentos.

    // Reemplazar el proceso actual con el nuevo ejecutable.
    if (Execpve(arguments, environVarsCount > 0 ? environVars : NULL, NULL, directoryList) == -1) {
        perror("Error ejecutando programa");
        liberarEnvironVars(&environVarsCount, environVars);
        _exit(-1);
    }
    liberarEnvironVars(&environVarsCount, environVars);
}

void command_execpri(char *pieces[], DirectoryList *directoryList, char *env[]) {
    if (pieces[0] == NULL || pieces[1] == NULL) {
        fprintf(stderr, "Uso: execpri <prio> <prog> <args...>\n");
        return;
    }

    long int priority = strtol(pieces[0], NULL, 10);
    char *environVars[64]; // Buffer para las variables de entorno.
    char *arguments[64];   // Buffer para los argumentos del ejecutable.
    int environVarsCount = 0, argumentsCount = 0;
    char *executableName = NULL;

    // Separar variables de entorno y argumentos.
    for (int i = 1; pieces[i] != NULL; i++) {
        int envIndex = BuscarVariable(pieces[i], env);
        if (envIndex != -1) {
            // Reconstruir la variable completa: "NOMBRE=valor".
            environVars[environVarsCount] = strdup(env[envIndex]);
            environVarsCount++;
        } else {
            if (argumentsCount == 0) {
                executableName = pieces[i]; // Primer argumento no es una variable, es el ejecutable.
            }
            arguments[argumentsCount++] = pieces[i];
        }
    }

    environVars[environVarsCount] = NULL; // Terminar lista de entorno.

    // Asegurarse de que arguments[0] contenga el nombre del ejecutable.
    if (executableName == NULL) {
        fprintf(stderr, "Error: No se especificó un ejecutable.\n");
        liberarEnvironVars(&environVarsCount, environVars);
        return;
    }

    arguments[argumentsCount] = NULL; // Terminar lista de argumentos.

    // Reemplazar el proceso actual con el nuevo ejecutable.
    if (Execpve(arguments, environVarsCount > 0 ? environVars : NULL, &priority, directoryList) == -1) {
        perror("Error ejecutando programa");
        liberarEnvironVars(&environVarsCount, environVars);
        _exit(-1);
    }
    liberarEnvironVars(&environVarsCount, environVars);
}


struct SEN {
    char *nombre; // Nombre de la señal, por ejemplo "INT"
    int senal;    // Número de la señal, por ejemplo SIGINT
};


/*las siguientes funciones nos permiten obtener el nombre de una senal a partir
del nÃºmero y viceversa */
static struct SEN sigstrnum[]={
    {"HUP", SIGHUP},
    {"INT", SIGINT},
    {"QUIT", SIGQUIT},
    {"ILL", SIGILL},
    {"TRAP", SIGTRAP},
    {"ABRT", SIGABRT},
    {"IOT", SIGIOT},
    {"BUS", SIGBUS},
    {"FPE", SIGFPE},
    {"KILL", SIGKILL},
    {"USR1", SIGUSR1},
    {"SEGV", SIGSEGV},
    {"USR2", SIGUSR2},
    {"PIPE", SIGPIPE},
    {"ALRM", SIGALRM},
    {"TERM", SIGTERM},
    {"CHLD", SIGCHLD},
    {"CONT", SIGCONT},
    {"STOP", SIGSTOP},
    {"TSTP", SIGTSTP},
    {"TTIN", SIGTTIN},
    {"TTOU", SIGTTOU},
    {"URG", SIGURG},
    {"XCPU", SIGXCPU},
    {"XFSZ", SIGXFSZ},
    {"VTALRM", SIGVTALRM},
    {"PROF", SIGPROF},
    {"WINCH", SIGWINCH},
    {"IO", SIGIO},
    {"SYS", SIGSYS},
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
    {"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
    {"PWR", SIGPWR},
#endif
#ifdef SIGEMT
    {"EMT", SIGEMT},
#endif
#ifdef SIGINFO
    {"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
    {"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
    {"CLD", SIGCLD},
#endif
#ifdef SIGLOST
    {"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
    {"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
    {"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
    {"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
    {"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
    {"WAITING", SIGWAITING},
#endif
     {NULL,-1},
    };    /*fin array sigstrnum */

char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/
{			/* para sitios donde no hay sig2str*/
    int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (sen==sigstrnum[i].senal)
            return sigstrnum[i].nombre;
    return ("SIGUNKNOWN");
}

/*Revisar valor de retorno porque cuando, back xterm -fg green -bg black -e /usr/local/bin/ksh, falla y se guarda en lista
 * porque en el valor de retorno está uno en vez de 255 como en shell de referencia */

void command_fg(char *pieces[], char *env[], DirectoryList *directoryList) {
    pid_t pid;
    char *environVars[64]; // Buffer para las variables de entorno.
    char *arguments[64]; // Buffer para los argumentos del ejecutable.
    int environVarsCount = 0, argumentsCount = 0, EnvIndex = 0;
    char *executableName = NULL;

    // Separar variables de entorno y argumentos.
    for (int i = 0; pieces[i] != NULL; i++) {
        EnvIndex = BuscarVariable(pieces[i], env);
        if (EnvIndex != -1) {
            // Reconstruir la variable completa: "NOMBRE=valor".
            environVars[environVarsCount] = strdup(env[EnvIndex]);
            environVarsCount++;
        } else {
            if (argumentsCount == 0) {
                executableName = pieces[i]; // Primer argumento no es una variable, es el ejecutable.
            }
            arguments[argumentsCount++] = pieces[i];
        }
    }

    environVars[environVarsCount] = NULL; // Terminar lista de entorno.

    // Asegurarse de que arguments[0] contenga el nombre del ejecutable.
    if (executableName != NULL) {
        arguments[0] = executableName;
    } else {
        fprintf(stderr, "Error: No se especificó un ejecutable.\n");
        return;
    }

    arguments[argumentsCount] = NULL; // Terminar lista de argumentos.

    // Crear proceso hijo.
    if ((pid = fork()) == 0) {
        // Proceso hijo: Ejecutar el programa.
        if (Execpve(arguments, environVarsCount > 0 ? environVars : NULL, NULL, directoryList) == -1) {
            perror("Error ejecutando programa");
            liberarEnvironVars(&environVarsCount, environVars);
            _exit(-1);
        }
    } else if (pid > 0) {
        // Proceso padre: Esperar a que el hijo termine.
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("Error esperando al proceso hijo");
        } else if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);
            printf("El proceso fue terminado por señal: %s\n", NombreSenal(signal));
        }
        liberarEnvironVars(&environVarsCount, environVars);
    } else {
        liberarEnvironVars(&environVarsCount, environVars);
        perror("Error creando el proceso");
    }
}

void command_fgpri(char *pieces[], char *env[], DirectoryList *directoryList) {
    pid_t pid;
    char *environVars[64]; // Buffer para las variables de entorno.
    char *arguments[64]; // Buffer para los argumentos del ejecutable.
    int environVarsCount = 0, argumentsCount = 0, EnvIndex = 0;      //environ empieza en -1 para empezar a escribir en 0 y arguments en 0 para empezar a escribir en 1 ya que hay que dejar hueco para el nombre del ejecutable
    char *executableName = NULL;
    long int prio = 0;

    // Validar que la prioridad esté en pieces[1] y sea un número válido.

    if (pieces[0] != NULL) {
        char *endptr;
        prio = strtol(pieces[0], &endptr, 10); // Usar strtol para validar el número.
        if (*endptr != '\0') {
            fprintf(stderr, "Error: La prioridad '%s' no es un número válido.\n", pieces[0]);
            return;
        }
    }else
        return;

    // Separar variables de entorno y argumentos.
    for (int i = 1; pieces[i] != NULL; i++) {
        // Si la variable ya ha sido agregada, no se vuelve a agregar.
        EnvIndex = BuscarVariable(pieces[i], env);
        if (EnvIndex != -1) {
            // Reconstruir la variable completa: "NOMBRE=valor".
            environVars[environVarsCount] = strdup(env[EnvIndex]);
            environVarsCount++;
        } else {
            if (argumentsCount == 0) {
                executableName = pieces[i]; // Primer argumento no es una variable, es el ejecutable.
            }
            arguments[argumentsCount++] = pieces[i];
        }
    }

    environVars[environVarsCount] = NULL; // Terminar lista de entorno.

    // Asegurarse de que arguments[0] contenga el nombre del ejecutable.
    if (executableName != NULL) {
        arguments[0] = executableName;
    } else {
        fprintf(stderr, "Error: No se especificó un ejecutable.\n");
        liberarEnvironVars(&environVarsCount, environVars);
        return;
    }

    arguments[argumentsCount] = NULL; // Terminar lista de argumentos.

    // Crear proceso hijo.
    if ((pid = fork()) == 0) {
        // Proceso hijo: Ejecutar el programa.
        if (Execpve(arguments, environVarsCount > 0 ? environVars : NULL, &prio, directoryList) == -1) {
            perror("Error ejecutando programa");
            liberarEnvironVars(&environVarsCount, environVars);
            _exit(-1);
        }
    } else if (pid > 0) {
        // Proceso padre: Esperar a que el hijo termine.
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("Error esperando al proceso hijo");
        } else if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);
            printf("El proceso fue terminado por señal: %s\n", NombreSenal(signal));
        }
        liberarEnvironVars(&environVarsCount, environVars);
    } else {
        liberarEnvironVars(&environVarsCount, environVars);
        perror("Error creando el proceso");
    }


}
void command_back(char *pieces[], char *env[], DirectoryList *directoryList, ProcessList *processList) {
    pid_t pid;
    char *environVars[64]; // Buffer para las variables de entorno.
    char *arguments[64]; // Buffer para los argumentos del ejecutable.
    int environVarsCount = 0, argumentsCount = 0, EnvIndex = 0;
    char *executableName = NULL;

    char fullcommand[1024] = "";

    // Separar variables de entorno y argumentos.
    for (int i = 0; pieces[i] != NULL; i++) {
        strcat(fullcommand, pieces[i]); // Agregar cada parte del comando a fullCommand.
        strcat(fullcommand, " ");      // Agregar un espacio entre los argumentos.

        EnvIndex = BuscarVariable(pieces[i], env);
        if (EnvIndex != -1) {
            // Reconstruir la variable completa: "NOMBRE=valor".
            environVars[environVarsCount] = strdup(env[EnvIndex]);
            environVarsCount++;
        } else {
            if (argumentsCount == 0) {
                executableName = pieces[i]; // Primer argumento no es una variable, es el ejecutable.
            }
            arguments[argumentsCount++] = pieces[i];
        }
    }

    fullcommand[strlen(fullcommand) - 1] = '\0'; // Eliminar el espacio extra al final.
    environVars[environVarsCount] = NULL; // Terminar lista de entorno.

    // Asegurarse de que arguments[0] contenga el nombre del ejecutable.
    if (executableName != NULL) {
        arguments[0] = executableName;
    } else {
        fprintf(stderr, "Error: No se especificó un ejecutable.\n");
        liberarEnvironVars(&environVarsCount, environVars);
        return;
    }

    arguments[argumentsCount] = NULL; // Terminar lista de argumentos.

    // Crear proceso hijo.
    if ((pid = fork()) == 0) {
        // Proceso hijo: Ejecutar el programa.
        if (Execpve(arguments, environVarsCount > 0 ? environVars : NULL, NULL, directoryList) == -1) {
            perror("Error ejecutando programa");
            liberarEnvironVars(&environVarsCount, environVars);
            _exit(-1);
        }
        liberarEnvironVars(&environVarsCount, environVars);

    }else if (pid > 0) {
        // Proceso padre: Añadir proceso a la lista.
        if (!addProcess(processList, pid, fullcommand)) {                           //Se añade el proceso del background en la lista con su pid y el nombre del comando ejecutado
            fprintf(stderr, "Error añadiendo el proceso a la lista.\n");
        }
        liberarEnvironVars(&environVarsCount, environVars);
    }else {
        liberarEnvironVars(&environVarsCount, environVars);
        perror("Error creando el proceso");
    }
}

void command_backpri(char *pieces[], char *env[], DirectoryList *directoryList, ProcessList *processList) {
    pid_t pid;
    char *environVars[64]; // Buffer para las variables de entorno.
    char *arguments[64]; // Buffer para los argumentos del ejecutable.
    int environVarsCount = 0, argumentsCount = 0, EnvIndex = 0;
    char *executableName = NULL;
    long int prio = 0;

    char fullcommand[1024] = "";

    // Validar que la prioridad esté en pieces[1] y sea un número válido.
    if (pieces[0] != NULL) {
        char *endptr;
        prio = strtol(pieces[0], &endptr, 10); // Usar strtol para validar el número.
        if (*endptr != '\0') {
            fprintf(stderr, "Error: La prioridad '%s' no es un número válido.\n", pieces[0]);
            return;
        }
    }else
        return;

    // Separar variables de entorno y argumentos.
    for (int i = 1; pieces[i] != NULL; i++) {
        strcat(fullcommand, pieces[i]); // Agregar cada parte del comando a fullCommand.
        strcat(fullcommand, " ");      // Agregar un espacio entre los argumentos.

        EnvIndex = BuscarVariable(pieces[i], env);
        if (EnvIndex != -1) {
            // Reconstruir la variable completa: "NOMBRE=valor".
            environVars[environVarsCount] = strdup(env[EnvIndex]);
            environVarsCount++;
        } else {
            if (argumentsCount == 0) {
                executableName = pieces[i]; // Primer argumento no es una variable, es el ejecutable.
            }
            arguments[argumentsCount++] = pieces[i];
        }
    }

    fullcommand[strlen(fullcommand) - 1] = '\0'; // Eliminar el espacio extra al final.
    environVars[environVarsCount] = NULL; // Terminar lista de entorno.

    // Asegurarse de que arguments[0] contenga el nombre del ejecutable.
    if (executableName != NULL) {
        arguments[0] = executableName;
    } else {
        fprintf(stderr, "Error: No se especificó un ejecutable.\n");
        liberarEnvironVars(&environVarsCount, environVars);
        return;
    }

    arguments[argumentsCount] = NULL; // Terminar lista de argumentos.

    // Crear proceso hijo.
    if ((pid = fork()) == 0) {
        // Proceso hijo: Ejecutar el programa.
        if (Execpve(arguments, environVarsCount > 0 ? environVars : NULL, &prio, directoryList) == -1) {
            perror("Error ejecutando programa");
            liberarEnvironVars(&environVarsCount, environVars);
            _exit(-1);
        }
        liberarEnvironVars(&environVarsCount, environVars);
    }else if (pid > 0) {
        // Proceso padre: Añadir proceso a la lista.
        if (!addProcess(processList, pid, fullcommand)) {
            fprintf(stderr, "Error añadiendo el proceso a la lista.\n");
        }
        liberarEnvironVars(&environVarsCount, environVars);
    } else {
        liberarEnvironVars(&environVarsCount, environVars);
        perror("Error creando el proceso");
    }
}

void command_listjobs(ProcessList *processList) {
    if (isEmptyListP(*processList)) {
        printf("No hay processos en segundo plano\n");
    }else {
        listJobs(*processList);
    }
}

void command_deljobs(ProcessList *processList) {
    if (isEmptyListP(*processList)) {
        printf("No hay processos en segundo plano\n");
    }else {
        delJobs(processList);
    }
}

void executeExternalCommand(char *pieces[], char *env[], DirectoryList *directoryList) {
    command_fg(pieces, env, directoryList);
}

/*QUE COJONES
 * pablojhd@asus:~/Escritorio/SO/P01$ valgrind --leak-check=full ./p3
==14368== Memcheck, a memory error detector
==14368== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==14368== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==14368== Command: ./p3
==14368==
Iniciando shell..
→ back xterm -fg green -bg black -e /usr/local/bin/ksh
Error ejecutando programa: No such file or directory
==14370==
==14370== HEAP SUMMARY:
==14370==     in use at exit: 21,788 bytes in 53 blocks
==14370==   total heap usage: 57 allocs, 4 frees, 25,332 bytes allocated
==14370==
==14370== LEAK SUMMARY:
==14370==    definitely lost: 0 bytes in 0 blocks
==14370==    indirectly lost: 0 bytes in 0 blocks
==14370==      possibly lost: 0 bytes in 0 blocks
==14370==    still reachable: 21,788 bytes in 53 blocks
==14370==         suppressed: 0 bytes in 0 blocks
==14370== Reachable blocks (those to which a pointer was found) are not shown.
==14370== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==14370==
==14370== For lists of detected and suppressed errors, rerun with: -s
==14370== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
→ listjobs
 14370   pablojhd p=-1 2024/12/09 21:21:02 TERMINADO (001) xterm -fg green -bg black -e /usr/local/bin/ksh
→ search -path
Importados 10 directorios en la ruta de busqueda
→ back xterm -fg green -bg black -e /usr/local/bin/ksh
→ back xterm -fg green -bg black -e /usr/local/bin/ksh
→ listjobs
 14370   pablojhd p=-1 2024/12/09 21:21:02 TERMINADO (001) xterm -fg green -bg black -e /usr/local/bin/ksh
 14389   pablojhd p=-1 2024/12/09 21:21:44 TERMINADO (000) xterm -fg green -bg black -e /usr/local/bin/ksh
 14391   pablojhd p=-1 2024/12/09 21:21:50 TERMINADO (000) xterm -fg green -bg black -e /usr/local/bin/ksh
→ exit
Saliendo de la shell...
==14368==
==14368== HEAP SUMMARY:
==14368==     in use at exit: 0 bytes in 0 blocks
==14368==   total heap usage: 115 allocs, 115 frees, 60,004 bytes allocated
==14368==
==14368== All heap blocks were freed -- no leaks are possible
==14368==
==14368== For lists of detected and suppressed errors, rerun with: -s
==14368== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

 */



