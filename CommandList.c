#include "CommandList.h"


// Función auxiliar para inicializar la lista con comandos predefinidos
void initializeCommandList(CommandList *list) {
    // Lista predefinida de comandos
    Command ComandosPredefinidos[] = {
        { "authors",   command_authors },
        { "pid",   command_pid },
        { "ppid",   command_ppid },
        { "cd",   command_cd },
        { "date",   command_date },
        { "historic",   command_historic },
        { "open",   command_open },
        { "close", command_close },
        { "dup", command_dup },
        { "infosys", command_infosys },
        { "help", command_help },
        { "quit", command_exit },
        { "exit", command_exit },
        { "bye", command_exit }
    };
    list->total = sizeof(ComandosPredefinidos) / sizeof(Command);
    for (int i = 0; i < list->total; i++)
        list->commands[i] = ComandosPredefinidos[i];
}

