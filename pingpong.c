//
// Copyright [Cuppari, Franco; 2023]
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>

void print_common_lines(int fork_result, int pid, int parent_pid);
void
print_common_lines(int fork_result, int pid, int parent_pid)
{
	printf("Donde fork me devuelve %d:\n", fork_result);
	printf("    - getpid me devuelve: %d\n", pid);
	printf("    - getppid me devuelve: %d\n", parent_pid);
}

int
main(void)
{
	int fds1[2];
	int fds2[2];
	int pipe1 = pipe(fds1);
	if (pipe1 < 0) {
		printf("Error en el primer pipe");
	}
	int pipe2 = pipe(fds2);
	if (pipe2 < 0) {
		printf("Error en el segundo pipe");
	}
	printf("Hola, soy PID %d\n",
	       getpid());  //  Por ahora tengo un único proceso
	printf("    - El primer pipe me devuelve [%d, %d]\n", fds1[0], fds1[1]);
	printf("    - El segundo pipe me devuelve [%d, %d]\n", fds2[0], fds2[1]);

	int i = fork();
	if (i < 0) {
		perror("Error en el fork");
		exit(-1);
	}
	if (i == 0) {            //  Proceso hijo
		close(fds1[1]);  //  No voy a escribir en este proceso en el fds1;
		close(fds2[0]);  //  No voy a leer en este proceso en el fds2;
		print_common_lines(i, getpid(), getppid());
		int received = 0;
		if (read(fds1[0], &received, sizeof(received)) < 0) {
			perror("Error en el pipe al recibir número");
			exit(-1);
		}
		printf("   - recibo valor %d via fd= %d\n", received, fds1[0]);
		close(fds1[0]);
		if (write(fds2[1], &received, sizeof(received)) < 0) {
			perror("Error en la escritura");
			exit(-1);
		}
		printf("    - reenvío valor %d a través de fd= %d y termino\n",
		       received,
		       fds2[1]);
		close(fds2[1]);

	} else {
		close(fds1[0]);  //  No voy a leer en este proceso en el fds1
		close(fds2[1]);  //  No voy a escribir en este proceso en el fds2
		print_common_lines(i, getpid(), getppid());
		srandom(time(NULL));
		int rand_num = random();
		printf("    - random me devuelve %d\n", rand_num);
		if (write(fds1[1], &rand_num, sizeof(rand_num)) < 0) {
			perror("Error en la escritura");
			exit(-1);
		}
		printf("    - envío valor %d a través de fd= %d\n",
		       rand_num,
		       fds1[1]);
		close(fds1[1]);
		int received = 0;
		if (read(fds2[0], &received, sizeof(received)) < 0) {
			perror("Error en la lectura");
			exit(-1);
		}
		printf("Hola, de nuevo PID %d:\n", getpid());
		printf("    - recibi valor: %d via %d\n", received, fds2[0]);
		close(fds2[0]);
		wait(NULL);
	}
	exit(0);
}