//
// Created by pablojhd on 16/09/24.
//

#include "functions.h"

/*las funciones entre puntos hay que implementarlas */

//Cada funcion es un comando

void imprimirPromt(){
    printf("→ ");
}

int TrocearCadena(char * cadena, char * trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

void leerEntrada(bool *terminado, CommandList *commands, HistoryList *history){
    char input[LENGTH_MAX];     //Simplemente sirve para leer la cadena, después de trocearla solo guarda el primer trozo
    if (fgets(input,sizeof(input),stdin) != NULL) {
        char *trozos[LENGTH_MAX];   //Array de strings, es decir, en cada posición se guarda un trozo de la entrada
        const char cadena = input;
        printf("La cadena es %s\n",input);
        int NumTrozos=TrocearCadena(input,trozos);  //Splitea la cadena en trozos
        if (NumTrozos>0) {
            printf("Primer trozo es: %s\n",trozos[0]);
            printf("Segundo trozo es: %s\n",trozos[1]);
            printf("Tercer trozo es: %s\n",trozos[2]);
            printf("Size %lu\n",sizeof(input));
            printf("NumTrozos=%d\n",NumTrozos);
            procesarEntrada(&cadena,trozos,NumTrozos,commands,history);
        }
    }else
        perror ("Error al leer la entrada");
}

void procesarEntrada(const char *cadena,const char trozos[LENGTH_MAX],int NumTrozos,CommandList *lista,HistoryList *history) {
    for (int i=0; i<lista->total ;i++) {
        if(strcmp(lista->commands[i].name,&trozos[0]) == 0){
            AddToHistoryList(&cadena,history);                  //Se guarda en el historial
            lista->commands->func(&trozos);
        }
        perror("Comando no válido, introduce ""help"" para ver los disponibles");
    }
}

void AddToHistoryList(char *command,HistoryList *lista){
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



void command_historic (char trozos[LENGTH_MAX], int NumTrozos) {


}

void command_open(char * tr[]) {

}
void command_close(char * tr[]) {

}
void command_dup(char * tr[]) {

}