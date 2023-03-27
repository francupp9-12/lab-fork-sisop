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
	int counter = 0;
	size_t tam = 0;
	char **arguments = calloc(NARGS + 2, sizeof(char *));
	arguments[0] = path;
	arguments[NARGS + 1] = NULL;  // Termino en NULL
	bool is_reading = true;
	while (is_reading) {
		char *line = NULL;
		int read = getline(&line, &tam, stdin);
		if (read != -1) {
			counter++;
			arguments[counter] = strtok(line, "\n");
		} else {
			is_reading = false;
		}
		if ((counter == NARGS) ||
		    ((is_reading == false) & (counter != 0))) {
			counter = 0;
			int f = fork();
			if (f < 0) {
				perror("Error en fork");
				exit(-1);
			}
			if (f == 0) {
				if (execvp(path, arguments) == -1) {
					perror("No se pudo ejecutar el "
					       "comando.");
					exit(-1);
				}
			} else {
				for (int i = 1; i <= NARGS; i++) {
					arguments[i] = NULL;
				}
				waitpid(f, NULL, 0);
			}
		}
		free(line);
	}
	free(arguments);
	// free(path);
	exit(0);
}