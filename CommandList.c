//Inicializa la lista de comandos disponibles

#include "CommandList.h"

void initializeCommandList(CommandList *list) {
    // Lista predefinida de comandos
    Command ComandosPredefinidos[] = {
        { "authors","Prints the names and logins of the program authors. authors -l prints only the logins and authors -n prints only the names.",   command_authors },
        { "pid","Prints the pid of the process executing the shell.",   command_pid },
        { "ppid","Prints the pid of the shell’s parent process.",   command_ppid },
        { "cd","Changes the current working directory of the shell to dir. When invoked without auguments it prints thecurrent working directory",   command_cd },
        { "date","Prints the current date and the current time. date -d Prints only the current date. date -t Prints only the current time ",   command_date },
        { "historic","Shows the historic of commands executed by this shell. historic Prints all the comands that have been input with their order number. historic N Repeats command number N. historic -N Prints only the lastN comands ",   command_historic },
        { "open","open without arguments lists the shell open files. open file, Opens a file and adds it to the list of shell open files",   command_open },
        { "close","Closes the df file descriptor and eliminates the corresponding item from the list", command_close },
        { "dup","Duplicates the df file descriptor creating the corresponding new entry on the file list", command_dup },
        { "infosys","Prints information on the machine running the shell", command_infosys },
        { "help","help displays a list of available commands. help cmd gives a brief help on the usage of comand cmd", command_help },
        { "quit", "Ends the shell",command_exit },
        { "exit", "Ends the shell",command_exit },
        { "bye", "Ends the shell",command_exit }
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

void freeDynamicMemory(void *dynamicMemoryPointer) {
    free(dynamicMemoryPointer);
}



