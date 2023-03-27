#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>

#ifndef NARGS
#define NARGS 4
#endif

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		perror("No recibió un comando");
		return -1;
	}
	char *path = argv[1];
	int contador = 0;
	size_t tam = 0;
	char **argumentos = calloc(NARGS + 2, sizeof(char *));
	argumentos[0] = path;
	argumentos[NARGS + 1] = NULL;
	bool hay_mas_lineas = true;
	while (hay_mas_lineas) {
		char *linea = NULL;
		int lectura = getline(&linea, &tam, stdin);
		if (lectura != -1) {
			contador++;
			argumentos[contador] = strtok(linea, "\n");
		} else {
			hay_mas_lineas = false;
		}
		if ((contador == NARGS) ||
		    ((hay_mas_lineas == false) & (contador != 0))) {
			contador = 0;
			int f = fork();
			if (f < 0) {
				perror("Error en fork");
				exit(-1);
			}
			if (f == 0) {
				if (execvp(path, argumentos) == -1) {
					perror("No se pudo ejecutar el "
					       "comando.");
					exit(-1);
				}
			} else {
				for (int i = 1; i <= NARGS; i++) {
					argumentos[i] = NULL;
				}
				waitpid(f, NULL, 0);
			}
		}
		free(linea);
	}
	free(argumentos);
	//free(path);
	exit(0);
}