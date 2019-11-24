#include <stdio.h> //biblioteca estandar
#include <unistd.h> //biblioteca para uso de fork (crear procesos)
#include <string.h> //Uso de exit
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

/* CONSTANTES */
#define PRMTSIZ 255
#define MAXARGS 63
#define MAXARGS2 10
#define EXITWORD "exit"
#define TUBERIA "|"
#define DIRECCIONA ">"
#define DIRECCIONAENT "<"
#define EVER ;;
#define NHILOS 10 //maximo 10 tuberias en una sentencia

void tuberia (char **args){ //El hilo lee el comando ingresado despues del |
//strncpy(lineaE->string, input, j);
    int pid, pid2, pid3, pip, fds[2], fds2[2], fds3[2];
    pip=pipe(fds);
    int i, j, h = 0;
    int posTuberias[5];
    char *comandos_solo1[MAXARGS2 + 1] = { NULL };
    char *comandos_solo2[MAXARGS2 + 1] = { NULL };
    char *comandos_solo3[MAXARGS2 + 1] = { NULL };
    char *comandos_solo4[MAXARGS2 + 1] = { NULL };
    for (i = 0; i < sizeof(args); i++){
        if(args[i] == NULL)
            break;
        if (strcmp(TUBERIA, args[i]) == 0){
            posTuberias[h] = i;
            if(h == 0) {
                for (j = 0; j < i; j++) {
                    comandos_solo1[j] = args[j];
                }
            }
            if(h == 1) {
                for (j = 0; j < i; j++) {
                    if(args[j+posTuberias[h-1]+1] == NULL || strcmp(TUBERIA, args[j+posTuberias[h-1]+1]) == 0)
                        break;
                    comandos_solo2[j] = args[j+posTuberias[h-1]+1];
                }
            }
            if(h == 2) {
                for (j = 0; j < i; j++) {
                    if(args[j+posTuberias[h-1]+1] == NULL || strcmp(TUBERIA, args[j+posTuberias[h-1]+1]) == 0)
                        break;
                    comandos_solo3[j] = args[j+posTuberias[h-1]+1];
                }
            }
            if(h == 3) {
                for (j = 0; j < i; j++) {
                    if(args[j+posTuberias[h-1]+1] == NULL || strcmp(TUBERIA, args[j+posTuberias[h-1]+1]) == 0)
                        break;
                    comandos_solo4[j] = args[j+posTuberias[h-1]+1];
                }
            }
            h++;
        }
    }

            if(h == 1) {
                for (j = posTuberias[h-1], i=0; j < sizeof(args); j++, i++) {
                    if(args[j+1] == NULL)
                        break;
                    comandos_solo2[i] = args[j+1];
                }
                if(pip ==-1) {
                    perror("\nError en pipe");
                    exit(pip);
                }
                pid=fork();
                if(pid ==-1) {
                    perror("\nError en fork");
                    exit(pid);
                }
                else if(pid==0){      // PROCESO B (proceso hijo)
                    close(0);		//cerrar entrada estandar
                    dup(fds[0]);	//duplicar RD de tuberia
                    //close(1);		//cerrar salida estandar
                    close(fds[0]);
                    close(fds[1]);
                    execvp(comandos_solo2[0], comandos_solo2);//Ejecutamos la linea
                 }
                else{			//PROCESO A (PROCESO padre)
                    close(1);		//cerrar salida estandar
                    dup(fds[1]);	//duplicar RW de tuberia
                    close(fds[0]);
                    close(fds[1]);
                    execvp(comandos_solo1[0], comandos_solo1);//Ejecutamos la linea
                }
            }
            if(h == 2) {
                for (j = posTuberias[h-1], i=0; j < sizeof(args); j++, i++) {
                    if(args[j+1] == NULL || strcmp(TUBERIA, args[j+1]) == 0)
                        break;
                    comandos_solo3[i] = args[j+1];
                }

                if (pip == -1) {
                    perror("\nError en pipe 1");
                    exit(1);
                }
                if ((pid = fork()) == -1) {
                    perror("\nError en fork 1");
                    exit(1);
                } else if (pid == 0) {
                    dup2(fds[1], 1);//duplicar RW de tuberia
                    close(fds[0]);//cerrar entrada estandar
                    close(fds[1]);//cerrar salida estandar
                    execvp(comandos_solo1[0], comandos_solo1);//Ejecutamos la linea
                }
                // Creando segunda tuberia
                if (pipe(fds2) == -1) {
                    perror("\nError en pipe 2");
                    exit(1);
                }
                if ((pid = fork()) == -1) {
                    perror("\nError en fork 2");
                    exit(1);
                } else if (pid == 0) {

                    dup2(fds[0], 0);//duplicar RD de tuberia1
                    // output to pipe2
                    dup2(fds2[1], 1);//duplicar RW de tuberia2
                    // close fds
                    close(fds[0]);//cerrar entrada estandar
                    close(fds[1]);//cerrar salida estandar
                    close(fds2[0]);//cerrar entrada estandar
                    close(fds2[1]);//cerrar salida estandar
                    execvp(comandos_solo2[0], comandos_solo2);//Ejecutamos la linea

                }
                // cerrando entradas y salidas ya no usadas
                close(fds[0]);
                close(fds[1]);

                if ((pid = fork()) == -1) {
                    perror("\nError en fork 3");
                    exit(1);
                } else if (pid == 0) {
                    // input from pipe2
                    dup2(fds2[0], 0);//duplicar RD de tuberia2
                    close(fds2[0]);//cerrar entrada estandar
                    close(fds2[1]);//cerrar salida estandar
                    execvp(comandos_solo3[0], comandos_solo3);//Ejecutamos la linea
                }
                wait(NULL); // esperar a que acabe todo xd
            }
            if(h == 3) {
                for (j = posTuberias[h-1], i=0; j < sizeof(args); j++, i++) {
                    if(args[j+1] == NULL)
                        break;
                    comandos_solo4[i] = args[j+1];
                }
            }
}

void lectura (char **args){ //Lee lo que sigue y la tuberia entonces lo escribe

    int pid, pid2, pid3, pip, fds[2], fds2[2], fds3[2];
    pip=pipe(fds);
    int i, j, h = 0;
    int posTuberias[5];
    char *comandos_solo1[MAXARGS2 + 1] = { NULL };
    char *comandos_solo2[MAXARGS2 + 1] = { NULL };
    char *comandos_solo3[MAXARGS2 + 1] = { NULL };
    char *comandos_solo4[MAXARGS2 + 1] = { NULL };
    char *comando_sin_pipes[MAXARGS2 + 1] = { NULL };
    char *nombre_archivo;
    for (i = 0; i < sizeof(args); i++){
        if(args[i] == NULL)
            break;
        if (strcmp(TUBERIA, args[i]) == 0){
            posTuberias[h] = i;
            if(h == 0) {
                for (j = 0; j < i; j++) {
                    comandos_solo1[j] = args[j];
                }
            }
            if(h == 1) {
                for (j = 0; j < i; j++) {
                    if(args[j+posTuberias[h-1]+1] == NULL || strcmp(TUBERIA, args[j+posTuberias[h-1]+1]) == 0 || strcmp(DIRECCIONAENT, args[j+posTuberias[h-1]+1]) == 0)
                        break;
                    comandos_solo2[j] = args[j+posTuberias[h-1]+1];
                }
            }
            if(h == 2) {
                for (j = 0; j < i; j++) {
                    if(args[j+posTuberias[h-1]+1] == NULL || strcmp(TUBERIA, args[j+posTuberias[h-1]+1]) == 0 || strcmp(DIRECCIONAENT, args[j+posTuberias[h-1]+1]) == 0)
                        break;
                    comandos_solo3[j] = args[j+posTuberias[h-1]+1];
                }
            }
            if(h == 3) {
                for (j = 0; j < i; j++) {
                    if(args[j+posTuberias[h-1]+1] == NULL || strcmp(TUBERIA, args[j+posTuberias[h-1]+1]) == 0 || strcmp(DIRECCIONAENT, args[j+posTuberias[h-1]+1]) == 0)
                        break;
                    comandos_solo4[j] = args[j+posTuberias[h-1]+1];
                }
            }
            h++;
        }

        if (strcmp(DIRECCIONAENT, args[i]) == 0) {
            if(args[i+1] != NULL && args[i+2] == NULL) {
                nombre_archivo = args[i+1];
                for (j = 0; j < i; j++) {
                    comando_sin_pipes[j] = args[j];
                }
            }
            else
                exit(-1);
        }
    }

    if(h == 0) {
        pid=fork();
        if(pid ==-1) {
            perror("\nError en fork");
            exit(pid);
        }
        else if(pid==0){      // PROCESO B (proceso hijo)
            close(0);		//cerrar entrada estandar
            open(nombre_archivo, O_RDONLY);	//crea o abre archivo
            execvp(comando_sin_pipes[0], comando_sin_pipes);//Ejecutamos la linea
        }

        wait(NULL);
    }
    if(h == 1) {
        for (j = posTuberias[h-1], i=0; j < sizeof(args); j++, i++) {
            if(args[j+1] == NULL || strcmp(DIRECCIONAENT, args[j+1]) == 0)
                break;
            comandos_solo2[i] = args[j+1];
        }
        if(pip ==-1) {
            perror("\nError en pipe");
            exit(pip);
        }
        pid=fork();
        if(pid ==-1) {
            perror("\nError en fork");
            exit(pid);
        }
        else if(pid==0){      // PROCESO B (proceso hijo)
            close(0);		//cerrar entrada estandar
            dup(fds[0]);	//duplicar RD de tuberia
            close(0);
            open(nombre_archivo, O_RDONLY);	//crea o abre archivo
            close(fds[0]);
            close(fds[1]);
            execvp(comandos_solo2[0], comandos_solo2);//Ejecutamos la linea
        }
        else{			//PROCESO A (PROCESO padre)
            close(1);		//cerrar salida estandar
            dup(fds[1]);	//duplicar RW de tuberia
            close(fds[0]);
            close(fds[1]);
            execvp(comandos_solo1[0], comandos_solo1);//Ejecutamos la linea
        }
    }
    if(h == 2) {
        for (j = posTuberias[h-1], i=0; j < sizeof(args); j++, i++) {
            if(args[j+1] == NULL || strcmp(TUBERIA, args[j+1]) == 0 || strcmp(DIRECCIONAENT, args[j+1]) == 0)
                break;
            comandos_solo3[i] = args[j+1];
        }
        if (pip == -1) {
            perror("\nError en pipe 1");
            exit(1);
        }
        if ((pid = fork()) == -1) {
            perror("\nError en fork 1");
            exit(1);
        } else if (pid == 0) {
            dup2(fds[1], 1);//duplicar RW de tuberia
            close(fds[0]);//cerrar entrada estandar
            close(fds[1]);//cerrar salida estandar
            execvp(comandos_solo1[0], comandos_solo1);//Ejecutamos la linea
        }
        // Creando segunda tuberia
        if (pipe(fds2) == -1) {
            perror("\nError en pipe 2");
            exit(1);
        }
        if ((pid = fork()) == -1) {
            perror("\nError en fork 2");
            exit(1);
        } else if (pid == 0) {

            dup2(fds[0], 0);//duplicar RD de tuberia1
            // output to pipe2
            dup2(fds2[1], 1);//duplicar RW de tuberia2
            // close fds
            close(fds[0]);//cerrar entrada estandar
            close(fds[1]);//cerrar salida estandar
            close(fds2[0]);//cerrar entrada estandar
            close(fds2[1]);//cerrar salida estandar
            execvp(comandos_solo2[0], comandos_solo2);//Ejecutamos la linea
        }
        // cerrando entradas y salidas ya no usadas
        close(fds[0]);
        close(fds[1]);

        if ((pid = fork()) == -1) {
            perror("\nError en fork 3");
            exit(1);
        } else if (pid == 0) {
            // input from pipe2
            dup2(fds2[0], 0);//duplicar RD de tuberia2
            close(fds2[0]);//cerrar entrada estandar
            close(fds2[1]);//cerrar salida estandar
            close(0);
            open(nombre_archivo, O_RDONLY);	//crea o abre archivo
            execvp(comandos_solo3[0], comandos_solo3);//Ejecutamos la linea
        }
        wait(NULL); // esperar a que acabe todo xd
    }
}

void direccionar (char **args){ //Escribe directo para una tuberia

    int pid, pid2, pid3, pip, fds[2], fds2[2], fds3[2];
    pip=pipe(fds);
    int i, j, h = 0;
    int posTuberias[5];
    char *comandos_solo1[MAXARGS2 + 1] = { NULL };
    char *comandos_solo2[MAXARGS2 + 1] = { NULL };
    char *comandos_solo3[MAXARGS2 + 1] = { NULL };
    char *comandos_solo4[MAXARGS2 + 1] = { NULL };
    char *comando_sin_pipes[MAXARGS2 + 1] = { NULL };
    char *nombre_archivo;
    for (i = 0; i < sizeof(args); i++){
        if(args[i] == NULL)
            break;
        if (strcmp(TUBERIA, args[i]) == 0){
            posTuberias[h] = i;
            if(h == 0) {
                for (j = 0; j < i; j++) {
                    if(strcmp(DIRECCIONA, args[j]) == 0)
                        break;
                    comandos_solo1[j] = args[j];
                }
            }
            if(h == 1) {
                for (j = 0; j < i; j++) {
                    if(args[j+posTuberias[h-1]+1] == NULL || strcmp(TUBERIA, args[j+posTuberias[h-1]+1]) == 0 || strcmp(DIRECCIONA, args[j+posTuberias[h-1]+1]) == 0)
                        break;
                    comandos_solo2[j] = args[j+posTuberias[h-1]+1];
                }
            }
            if(h == 2) {
                for (j = 0; j < i; j++) {
                    if(args[j+posTuberias[h-1]+1] == NULL || strcmp(TUBERIA, args[j+posTuberias[h-1]+1]) == 0 || strcmp(DIRECCIONA, args[j+posTuberias[h-1]+1]) == 0)
                        break;
                    comandos_solo3[j] = args[j+posTuberias[h-1]+1];
                }
            }
            if(h == 3) {
                for (j = 0; j < i; j++) {
                    if(args[j+posTuberias[h-1]+1] == NULL || strcmp(TUBERIA, args[j+posTuberias[h-1]+1]) == 0 || strcmp(DIRECCIONA, args[j+posTuberias[h-1]+1]) == 0)
                        break;
                    comandos_solo4[j] = args[j+posTuberias[h-1]+1];
                }
            }
            h++;
        }

        if (strcmp(DIRECCIONA, args[i]) == 0) {
            if(args[i+1] != NULL && args[i+2] == NULL) {
                nombre_archivo = args[i+1];
                for (j = 0; j < i; j++) {
                    comando_sin_pipes[j] = args[j];
                }
            }
            else
                exit(-1);
        }
    }

    if(h == 0) {
        close(1);
        open(nombre_archivo, O_WRONLY | O_CREAT | O_TRUNC ,S_IRWXU );	//crea o abre archivo
        execvp(comando_sin_pipes[0], comando_sin_pipes);//Ejecutamos la linea
    }

    if(h == 1) {
        for (j = posTuberias[h-1], i=0; j < sizeof(args); j++, i++) {
            if(args[j+1] == NULL || strcmp(DIRECCIONA, args[j+1]) == 0)
                break;
            comandos_solo2[i] = args[j+1];
        }
        if(pip ==-1) {
            perror("\nError en pipe");
            exit(pip);
        }
        pid=fork();
        if(pid ==-1) {
            perror("\nError en fork");
            exit(pid);
        }
        else if(pid==0){      // PROCESO B (proceso hijo)
            close(0);		//cerrar entrada estandar
            dup(fds[0]);	//duplicar RD de tuberia
            close(1);		//cerrar salida estandar
            open(nombre_archivo, O_WRONLY | O_CREAT | O_TRUNC ,S_IRWXU );	//crea o abre archivo
            close(fds[0]);
            close(fds[1]);
            execvp(comandos_solo2[0], comandos_solo2);//Ejecutamos la linea
        }
        else{			//PROCESO A (PROCESO padre)
            close(1);		//cerrar salida estandar
            dup(fds[1]);	//duplicar RW de tuberia
            close(fds[0]);
            close(fds[1]);
            execvp(comandos_solo1[0], comandos_solo1);//Ejecutamos la linea
        }
    }
    if(h == 2) {
        for (j = posTuberias[h-1], i=0; j < sizeof(args); j++, i++) {
            if(args[j+1] == NULL || strcmp(TUBERIA, args[j+1]) == 0 || strcmp(DIRECCIONA, args[j+1]) == 0)
                break;
            comandos_solo3[i] = args[j+1];
        }
        if (pip == -1) {
            perror("\nError en pipe 1");
            exit(1);
        }
        if ((pid = fork()) == -1) {
            perror("\nError en fork 1");
            exit(1);
        } else if (pid == 0) {
            dup2(fds[1], 1);//duplicar RW de tuberia
            close(fds[0]);//cerrar entrada estandar
            close(fds[1]);//cerrar salida estandar
            execvp(comandos_solo1[0], comandos_solo1);//Ejecutamos la linea
        }
        // Creando segunda tuberia
        if (pipe(fds2) == -1) {
            perror("\nError en pipe 2");
            exit(1);
        }
        if ((pid = fork()) == -1) {
            perror("\nError en fork 2");
            exit(1);
        } else if (pid == 0) {

            dup2(fds[0], 0);//duplicar RD de tuberia1
            // output to pipe2
            dup2(fds2[1], 1);//duplicar RW de tuberia2
            // close fds
            close(fds[0]);//cerrar entrada estandar
            close(fds[1]);//cerrar salida estandar
            close(fds2[0]);//cerrar entrada estandar
            close(fds2[1]);//cerrar salida estandar
            execvp(comandos_solo2[0], comandos_solo2);//Ejecutamos la linea
        }
        // cerrando entradas y salidas ya no usadas
        close(fds[0]);
        close(fds[1]);

        if ((pid = fork()) == -1) {
            perror("\nError en fork 3");
            exit(1);
        } else if (pid == 0) {
            // input from pipe2
            dup2(fds2[0], 0);//duplicar RD de tuberia2
            close(fds2[0]);//cerrar entrada estandar
            close(fds2[1]);//cerrar salida estandar
            close(1);		//cerrar salida estandar
            open(nombre_archivo, O_WRONLY | O_CREAT | O_TRUNC ,S_IRWXU );	//crea o abre archivo
            execvp(comandos_solo3[0], comandos_solo3);//Ejecutamos la linea
        }
        wait(NULL); // esperar a que acabe todo xd
    }
}

int main(void) {
    //tubos[0] descriptor de lectura
    //tubos[1] descriptor de escritura
    int bytesleidos;
    for (EVER) {
        char input[PRMTSIZ + 1] = { 0x0 }; //Iniciaizando arreglo con 0's
        char *ptrEntrada = input; //Puntero para recorrer los argumentos en consola
        char *ptrEntradaAux = input;
        char *args[MAXARGS + 1] = { NULL }; //Inicializando arreglo de argumentos en NULL
        char cwd[1024];
        //struct argsTub * lineaE = (struct argsTub *)malloc(sizeof(struct argsTub));
        getcwd(cwd, sizeof(cwd));
        // usuario:directorio/donde/se/encuentra$
        printf("usuario:~%s$ ", cwd);
        //Leyendo la cadena del usuario
        fgets(input, PRMTSIZ, stdin);

        // Ignorar si solo da enter
        if (*ptrEntrada == '\n')
            continue;

        // convert input line to list of arguments
        int hilito=0;
        int i;
        for (i = 0; i < sizeof(args) && *ptrEntrada; ptrEntrada++, ptrEntradaAux++) {

            if (*ptrEntrada == ' ') {
                continue; //hay un espacio? por ejemplo 'ls -l'
            }
            if (*ptrEntrada == '|') {
                hilito = 1;
            }
            if (*ptrEntrada == '<'){
                hilito = 2;
            }
            if (*ptrEntrada == '>'){
                hilito = 3;
            }


            if (*ptrEntrada == '\n') break; //Acaba la linea

            for (args[i++] = ptrEntrada; *ptrEntrada && *ptrEntrada != ' ' && *ptrEntrada != '\n'; ptrEntrada++);
            *ptrEntrada = '\0';
        }

        // si pone 'exit', terminar ejecucuión del programa
        if (strcmp(EXITWORD, args[0]) == 0) {
            printf("\nBYE\n\n");
            return 0;
        }

        signal(SIGINT, SIG_DFL);
        int pid;
        pid = fork();
        if (pid == -1) {
            perror("\nError en fork\n");
            exit(-1);
        } else if(pid == 0 && hilito == 0) {
            execvp(args[0], args);//Ejecutamos la linea
            exit(0);
        } else if(pid == 0 && hilito == 1) {
            tuberia(args);
            exit(0);
        } else if(pid == 0 && hilito == 2) {
            lectura(args);
            exit(0);
        } else if(pid == 0 && hilito == 3) {
            direccionar(args);
            exit(0);
        }

        signal(SIGINT, SIG_IGN);
        wait(NULL);
    }
}
