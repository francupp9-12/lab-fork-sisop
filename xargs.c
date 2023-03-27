#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

#ifndef NARGS
#define NARGS 4
#endif


void fork_proccess(char *commands[]);

void
fork_proccess(char *commands[])
{
    pid_t pidC;
    pidC = fork();
    if (pidC < 0) {
        perror("Error en el fork");
    }

    if (pidC > 0) {
        wait(NULL);
    } else if (pidC == 0) {
        execvp(commands[0], commands);
    }
}

int
main(int argc, char *argv[])
{
    char *line = NULL;
    size_t len = 0;

    char *commands[NARGS + 2] = { argv[1], NULL, NULL, NULL, NULL, NULL };

    if (argc < 2) {
        perror("Cantidad errónea de parámetros");
        exit(EXIT_FAILURE);
    }

    int iteracion = 1;

    while (getline(&line, &len, stdin) != -1) {
        line[strlen(line) - 1] = '\0';
        commands[iteracion] = strdup(line);
        if (iteracion == 4) {
            fork_proccess(commands);
            iteracion = 0;
        }

        iteracion++;
    }

    fork_proccess(commands);

    free(line);
    exit(EXIT_SUCCESS);
}