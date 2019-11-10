#include <stdio.h> //biblioteca estandar
#include <unistd.h> //biblioteca para uso de fork (crear procesos)
#include <string.h> //Uso de exit
#include <stdlib.h>
#include <sys/wait.h>

#define PRMTSIZ 255
#define MAXARGS 63
#define EXITWORD "exit"
#define EVER ;;

int main(void) {
    for (EVER) {
        char input[PRMTSIZ + 1] = { 0x0 }; //Iniciaizando arreglo con 0's
        char *ptrEntrada = input; //Puntero para recorrer los argumentos en consola
        char *args[MAXARGS + 1] = { NULL }; //Inicializando arreglo de argumentos en NULL


        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        // usuario:directorio/donde/se/encuentra$
        printf("usuario:%s$ ", cwd);
        //Leyendo la cadena del usuario
        fgets(input, PRMTSIZ, stdin);

        // Ignorar si solo da enter
        if (*ptrEntrada == '\n')
            continue;

        // convert input line to list of arguments
        for (int i = 0; i < sizeof(args) && *ptrEntrada; ptrEntrada++) {
            if (*ptrEntrada == ' ') continue; //hay un espacio? por ejemplo 'ls -l'
            if (*ptrEntrada == '\n') break; //Acaba la linea
            for (args[i++] = ptrEntrada; *ptrEntrada && *ptrEntrada != ' ' && *ptrEntrada != '\n'; ptrEntrada++);
            *ptrEntrada = '\0';
        }

        // si pone 'exit', terminar ejecucuión del programa
        if (strcmp(EXITWORD, args[0]) == 0) {
            printf("\n\nBYE\n\n");
            return 0;
        }

        // fork child and execute program
        signal(SIGINT, SIG_DFL);
        int pid;
        pid = fork();
        if (pid == -1) {
            perror("\nError en fork\n");
            exit(-1);
        } else if(pid == 0) {
            execvp(args[0], args);//Ejecutamos la linea
            exit(0);
        }
        signal(SIGINT, SIG_IGN);

        // wait for program to finish and print exit status
        wait(NULL);
    }
}
