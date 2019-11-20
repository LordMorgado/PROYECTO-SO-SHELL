#include <stdio.h> //biblioteca estandar
#include <unistd.h> //biblioteca para uso de fork (crear procesos)
#include <string.h> //Uso de exit
#include <stdlib.h>
#include <sys/wait.h>

#define PRMTSIZ 255
#define MAXARGS 63
#define EXITWORD "exit"
#define EVER ;;
#define NHILOS 10 //maximo 10 tuberias en una sentencia

void *tuberia (*void id){ //El hilo lee el comando ingresado despues del |
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char *ptrEntrada = input;
    int tube_pid = fork();
    if (tube_pid == -1) {
        perror("\nError en fork\n");
        exit(-1);
    } else if(tube_pid == 0) {
        execvp(args[0], args);//Ejecutamos la linea
        exit(0);
    }
}

int main(void) {
    int tubos[2];	//Descriptor de la tubería
    //tubos[0] descriptor de lectura
    //tubos[1] descriptor de escritura
    int bytesleidos;
    for (EVER) {
        char input[PRMTSIZ + 1] = { 0x0 }; //Iniciaizando arreglo con 0's
        char *ptrEntrada = input; //Puntero para recorrer los argumentos en consola
        char *args[MAXARGS + 1] = { NULL }; //Inicializando arreglo de argumentos en NULL
        int tubo=0;
        char ayudante[PRMTSIZ +1] = {0x0};
        int error, h, *valor, tubo;
        int id[NHILOS] = {1,2,3,4,5,6,7,8,9,10};
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
        int hilito=0;
        pthread hilos[NHILOS];
        for (int i = 0; i < sizeof(args) && *ptrEntrada; ptrEntrada++) {
            if (*ptrEntrada == ' ') continue; //hay un espacio? por ejemplo 'ls -l'
            if (*ptrEntrada == '|'){
                tubo=1;
                error=pthread_create(&hilos[hilito],i,tuberia,&id[hilito]); //Se manda con i
                if(error){
                    printf("Error al crear el hilo\n\n");
                }
                hilito++;
            }
            if (*ptrEntrada == '\n') break; //Acaba la linea
            for (args[i++] = ptrEntrada; *ptrEntrada && *ptrEntrada != ' ' && *ptrEntrada != '\n'; ptrEntrada++);
            *ptrEntrada = '\0';
        }
        /*
        leera el mensaje del hilo desde un documento en caso de que exista
        if(tubo){
                pipe(tubos);//Crea la tuberia
                if (fork() == 0) {
                    close(tubos[0]); //Cierro tubería de lectura
                    write(tubos[1], mensaje1, sizeof(mensaje1));
                    write(tubos[1], &mensaje2, sizeof(mensaje2));
                    close(tubos[1]);
                } else {
                    //Soy el proceso padre
                    close(tubos[1]);
                    bytesleidos = read (tubos[0], &mensaje1, sizeof(mensaje1));
                    printf ("Bytes leidos: %d\n", bytesleidos);
                    printf ("Mensaje: %s\n", mensaje1);
                    bytesleidos = read (tubos[0], &mensaje2, sizeof(mensaje2));
                    printf ("Bytes leidos: %d\n", bytesleidos);
                    printf ("Mensaje: %d\n", mensaje2);
                    close (tubos[0]);
        }
        
        }*/

        // si pone 'exit', terminar ejecucuión del programa
        if (strcmp(EXITWORD, args[0]) == 0) {
            printf("\n\nBYE\n\n");
            return 0;
        }

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
        wait(NULL);
    }
}
