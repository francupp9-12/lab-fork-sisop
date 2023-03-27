#ifndef NARGS
#define NARGS 4
#endif
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void execute(char *args[]);
void clean_commands(char *args[], int i);

void
execute(char *args[])
{
    int i = fork();
    if (i < 0) {
        perror("Error en fork");
        exit(-1);
    }

    if (i == 0) {
        execvp(args[0], args);
    } else {
        wait(NULL);
    }
}

void
clean_commands(char *args[], int i)
{
    for (int j = 1; j < i; j++) {
        free(args[j]);
    }
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        perror("Erronea cantidad de parametros");
        exit(EXIT_FAILURE);
    }
    size_t len = 0;
    size_t read = 0;
    char *line = NULL;
    char *args[NARGS + 2] = { argv[1] };
    int n_commands = 1;
    while ((read = getline(&line, &len, stdin) != -1)) {
        if (n_commands == (NARGS + 1)) {  // ya lei NARGS veces
            args[n_commands] = NULL;
            execute(args);
            clean_commands(args, n_commands);
            n_commands = 1;
        }
        line[strlen(line) - 1] = '\0';
        args[n_commands] = strdup(line);
        n_commands++;
    }
    if (n_commands > 1) {  //
        args[n_commands] = NULL;
        execute(args);
        clean_commands(args, n_commands);
    }

    free(line);
    exit(EXIT_SUCCESS);
}