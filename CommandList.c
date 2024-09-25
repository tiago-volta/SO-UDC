//
// Created by pablojhd on 22/09/24.
//

#include "CommandList.h"


// Función auxiliar para inicializar la lista con comandos predefinidos
void initializeCommandList(CommandList *list) {
    // Lista predefinida de comandos
    Command ComandosPredefinidos[] = {
        { "authors","Prints the names and logins of the program authors. authors -l prints only the logins and authors -n prints only the names.",   command_authors,1 },
        { "pid","Prints the pid of the process executing the shell.",   command_pid,0 },
        { "ppid","Prints the pid of the shell’s parent process.",   command_ppid,0 },
        { "cd","Changes the current working directory of the shell to dir. When invoked without auguments it prints thecurrent working directory",   command_cd,1 },
        { "date","Prints the current date and the current time. date -d Prints only the current date. date -t Prints only the current time ",   command_date,1 },
        { "historic","Shows the historic of commands executed by this shell. historic Prints all the comands that have been input with their order number. historic N Repeats command number N. historic -N Prints only the lastN comands ",   command_historic,2 },
        { "open","open without arguments lists the shell open files. open file, Opens a file and adds it to the list of shell open files",   command_open,2 },
        { "close","Closes the df file descriptor and eliminates the corresponding item from the list", command_close,2 },
        { "dup","Duplicates the df file descriptor creating the corresponding new entry on the file list", command_dup,2 },
        { "infosys","Prints information on the machine running the shell", command_infosys,0 },
        { "help","help displays a list of available commands. help cmd gives a brief help on the usage of comand cmd", command_help,2 },
        { "quit", "Ends the shell",command_exit,3 },
        { "exit", "Ends the shell",command_exit,3 },
        { "bye", "Ends the shell",command_exit,3 }
    };
    list->total = sizeof(ComandosPredefinidos) / sizeof(Command);
    for (int i = 0; i < list->total; i++)
        list->commands[i] = ComandosPredefinidos[i];
}

void printCommandList(CommandList list) {
    printf("Comandos disponibles:\n");
    for (int i = 0; i < list.total; i++) {
        printf("%s: %s\n",list.commands[i].name,list.commands[i].description);
    }
}
