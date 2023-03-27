#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>

void eratostenes(int *fds);

void
eratostenes(int *fds)
{
	int prime = 0;
	close(fds[1]);
	int r = read(fds[0], &prime, sizeof(prime));
	if (r < 0) {
		perror("Error en la lectura");
		exit(-1);
	} else if (r == 0) {
		close(fds[0]);
		exit(-1);
	}

	printf("primo %d\n", prime);

	int fds_bis[2];
	int r1 = pipe(fds_bis);
	if (r1 < 0) {
		perror("Error en pipe");
	}
	int i = fork();
	if (i < 0) {
		perror("Error en fork");
		exit(-1);
	}
	if (i == 0) {
		close(fds[0]);
		eratostenes(fds_bis);
		exit(0);
	} else {
		close(fds_bis[0]);
		bool cerrado = false;
		while (!cerrado) {
			int num = 0;
			int lectura = read(fds[0], &num, sizeof(num));
			if (lectura < 0) {
				perror("Error en la lectura");
				exit(-1);
			} else if (lectura == 0) {
				cerrado = true;
				close(fds[0]);
			}

			if ((num % prime) != 0) {
				if (write(fds_bis[1], &num, sizeof(num)) < 0) {
					perror("Error en la escritura");
					exit(-1);
				}
			}
		}
		close(fds_bis[1]);
		waitpid(i, NULL, 0);
		exit(0);
	}
}


int
main(int argc, char *argv[])
{
	if (argc != 2) {
		perror("No recibió un número");
		return -1;
	}

	int num = strtol(argv[1], NULL, 10);
	int fds[2];
	int res = pipe(fds);
	if (res < 0) {
		perror("Error en pipe");
	}
	int f = fork();
	if (f < 0) {
		perror("Error en fork");
		exit(-1);
	}
	if (f == 0) {
		eratostenes(fds);
	} else {
		for (int i = 2; i < num; i++) {
			int j = i;
			if (write(fds[1], &j, sizeof(j)) < 0) {
				perror("Error en la escritura");
				exit(-1);
			}
		}
		close(fds[1]);
		close(fds[0]);
		waitpid(f, NULL, 0);
	}
	exit(0);
}