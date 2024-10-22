/*
 * TITLE: Sistemas Operativos
 * SUBTITLE: Practica 0
 * AUTHOR 1: Pablo Herrero Diaz LOGIN 1: pablo.herrero.diaz
 * AUTHOR 2: Tiago Da Costa Teixeira Veloso E Volta LOGIN 2: tiago.velosoevolta
 * GROUP: 2.3
 * DATE: 27 / 09 / 24
 */

#include "Functions.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

//Imprime el prompt
void printPrompt(){
    printf("→ ");
    fflush(stdout);
}

//Funcion auxiliar para el read
int SplitString(char *str, char *pieces[]){
    int i=1;
    if ((pieces[0]=strtok(str," \n\t"))==NULL)
        return 0;
    while ((pieces[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

//Función auxiliar para guardar en el historial
static void AddToHistoryList(tItemH *command,HistoryList *lista){
    tItemH *newItem = command;
    insertCommandH(newItem,lista);
}


void readInput(bool *finished, CommandListC *commandList, HistoryList *history,OpenFileList *openFileList){
    char input[LENGTH_MAX_INPUT];
    if (fgets(input,LENGTH_MAX_INPUT,stdin) != NULL) {
        char *trozos[LENGTH_MAX_INPUT];
        tItemH cadena;
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

//Debería de poner los comandos aquí en una función que los devuelva con sus descripciones y luego inicializar la lista de comandos con una función del TAD?
void InsertPredefinedCommands(CommandListC *commandList) {
   const char *Names[] = {
        "authors", "pid", "ppid", "cd", "date",
        "historic", "open", "close", "dup", "infosys",
        "makefile", "makedir", "listfile", "cwd", "listdir",
        "reclist", "revlist", "erase", "delrec",
        "help", "quit", "exit", "bye"
    };
    const char *Descriptions[] = {
        " [-n|-l] Muestra los nombres y/o logins de los autores",
        " [-p] Muestra el pid del shell o de su proceso padre",
        " Muestra el pid del proceso padre del shell",
        " [dir]	Cambia (o muestra) el directorio actual del shell",
        " [-d|-t] Muestra la fecha y/o la hora actual",
        " [-c|-N|N]	Muestra (o borra)el historico de comandos \n"
        "\t-N: muestra los N primeros \n"
        "\t-c: borra el historico \n"
        "\tN: repite el comando N",
        " fich m1 m2...	Abre el fichero fich y lo anade a la lista de ficheros abiertos del shell \n"
        "\tm1, m2..es el modo de apertura (or bit a bit de los siguientes) \n"
        "\tcr: O_CREAT	ap: O_APPEND \n"
        "\tex: O_EXCL 	ro: O_RDONLY \n"
        "\trw: O_RDWR 	wo: O_WRONLY \n"
        "\ttr: O_TRUNC ",
        " df Cierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos",
        " df Duplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos",
        " Muestra informacion de la maquina donde corre el shell",
        " [name] Crea un fichero de nombre name",
        " [name]	Crea un directorio de nombre name",
        " [-long][-link][-acc] name1 name2 ..	lista ficheros; \n"
        "\t-long: listado largo \n"
        "\t-acc: acesstime \n"
        "\t-link: si es enlace simbolico, el path contenido ",
        "Muestra el directorio actual del shell",
        " [-hid][-long][-link][-acc] n1 n2 .. lista contenidos de directorios \n"
        "\t-long: listado largo \n"
        "\t-hid: incluye los ficheros ocultos \n"
        "\t-acc: acesstime \n"
        "\t-link: si es enlace simbolico, el path contenido",
        " [-hid][-long][-link][-acc] n1 n2 .. lista recursivamente contenidos de directorios (subdirs despues) \n"
        "\t-hid: incluye los ficheros ocultos \n"
        "\t-long: listado largo \n"
        "\t-acc: acesstime \n"
        "\t-link: si es enlace simbolico, el path contenido",
        " [-hid][-long][-link][-acc] n1 n2 .. lista recursivamente contenidos de directorios (subdirs antes) \n"
        "\t-hid: incluye los ficheros ocultos \n"
        "\t-long: listado largo \n"
        "\t-acc: acesstime \n"
        "\t-link: si es enlace simbolico, el path contenido ",
        " [name1 name2 ..] Borra ficheros o directorios vacios",
        " [name1 name2 ..] Borra ficheros o directorios no vacios recursivamente",
        " [cmd|-all]	Muestra ayuda sobre los comandos \n"
        "\t-cmd: info sobre el comando cmd \n"
        "\t-all: lista todos los comandos con su información ",
        " Termina la ejecucion del shell",
        " Termina la ejecucion del shell",
        " Termina la ejecucion del shell"
    };

    // Obtenemos el numero total de comandos dividiendo el tamaño total entre el tamaño de un comando
    int NumComandos = sizeof(Names) / sizeof(Names[0]);
    // Copiamos los valores en el struct CommandList
    for (int i = 0; i < NumComandos; i++) {
        if (!insertCommandC(commandList,Names[i],Descriptions[i],i))
            perror ("Error insertando los comandos predefinidos");
    }
}


//Obtenemos el ID del comando para luego poder elegir en el switch, además aprovechamos y guardamos en el historial
static int getCommandId (tItemH *str,char *pieces[],CommandListC *commandList,HistoryList *history) {
    int id = FindCommandC(commandList,pieces[0]);
    if (id != CNULL) {
        AddToHistoryList(str,history);
        return id;
    }
    return -1;
}

//Procesa el comando introducido //Se puede hacer privada??
void processInput(bool *finished,tItemH *str,char *pieces[], CommandListC *commandList, HistoryList *history,OpenFileList *fileList){
    switch (getCommandId(str,pieces,commandList,history)) {
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
    }
}

