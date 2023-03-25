//
// Copyright [Cuppari, Franco; 2023]
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <stdbool.h>

void erastotenes(int *fds);

void
erastotenes(int *fds)
{
	int prime = 0;
	close(fds[1]);  //  No escribo en este pipe.
	int ret = read(fds[0], &prime, sizeof(prime));
	if (ret < 0) {
		perror("Error en la lectura");
		exit(-1);
	} else if (ret == 0) {  //  EOF
		close(fds[0]);
		exit(-1);
	}
	printf("primo: %d\n", prime);
	int fds_bis[2];
	int result_bis = pipe(fds_bis);
	if (result_bis < 0) {
		perror("Error en pipe");
	}
	int f = fork();
	if (f < 0) {
		perror("Error en fork");
		exit(-1);
	}
	if (f == 0) {  //  Hijo
		close(fds[0]);
		erastotenes(fds_bis);
		exit(0);
	} else {
		close(fds_bis[0]);  //  Cierro lectura
		_Bool closed = false;
		while (!closed) {
			int num = 0;
			int reading = read(fds[0], &num, sizeof(num));
			if (reading < 0) {
				perror("Error en la lectura");
				exit(-1);
			} else if (reading == 0) {
				//  EOF
				closed = true;
				close(fds[0]);
			}
			if (num % prime != 0) {
				if (write(fds_bis[1], &num, sizeof(num)) < 0) {
					perror("Error en la escritura");
					exit(-1);
				}
			}
		}
	}
	close(fds_bis[1]);
	waitpid(f, NULL, 0);
	exit(0);
}


int
main(int argc, char *argv[])
{
	if (argc != 2) {
		perror("Número incorrecto de argumentos");
		return (-1);
	}
	long number = strtol(argv[1], NULL, 10);  //  Paso a base 10
	int fds[2];
	int result = pipe(fds);
	if (result < 0) {
		perror("Error en el pipe");
	}
	int f = fork();
	if (f < 0) {
		perror("Error en el fork");
		return (-1);
	}
	if (f == 0) {  //  Hijo
		erastotenes(fds);
	} else {  //  Proceso generador
		for (int i = 2; i < number; i++) {
			int aux = i;
			if (write(fds[1], &aux, sizeof(aux)) < 0) {
				perror("Error en la escritura");
				return (-1);
			}
		}
		close(fds[1]);
		close(fds[0]);
		waitpid(f, NULL, 0);
	}
	exit(0);
}
