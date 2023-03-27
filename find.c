#define _GNU_SOURCE
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

void search_in_directory(DIR *directory,
                         char *searching,
                         char *path,
                         char *(*f_str)(const char *, const char *) );

void
search_in_directory(DIR *directory,
                    char *searching,
                    char *path,
                    char *(*f_str)(const char *, const char *) )
{
	struct dirent *entry;
	while ((entry = readdir(directory))) {
		char *name = entry->d_name;
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
			continue;
		}
		if (entry->d_type == DT_REG) {
			if (f_str(name, searching) != NULL) {
				printf("%s%s\n", path, name);
				continue;
			}
		}
		if (entry->d_type == DT_DIR) {
			int fd = dirfd(directory);
			if (fd == -1) {
				perror("Error en dirfd.");
				continue;
			}
			int dir = openat(fd, name, O_DIRECTORY);
			if (dir == -1) {
				perror("Error en openat.");
				continue;
			}
			int len = strlen(path);
			path = strcat(path, name);
			if (f_str(name, searching) != NULL) {
				printf("%s\n", path);
			}
			path = strcat(path, "/");
			search_in_directory(fdopendir(dir), searching, path, f_str);
			path[len] = '\0';
			close(dir);
		}
	}
}

int
main(int argc, char *argv[])
{
	char *searching;
	char *(*func)(const char *, const char *);
	if (argc == 2) {
		searching = argv[1];
		func = strstr;
	} else if ((argc == 3) & (strcmp(argv[1], "-i") == 0)) {
		searching = argv[2];
		func = strcasestr;
	} else {
		perror("Cantidad de argumentos inadecuada");
		return 0;
	}
	DIR *directory = opendir(".");
	char path[PATH_MAX];
	path[0] = '\0';  // inicializo el buffer para evitar warnings
	search_in_directory(directory, searching, path, func);
	closedir(directory);
	exit(0);
}