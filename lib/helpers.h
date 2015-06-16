#ifndef HELPERS_H
#define HELPERS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>


ssize_t read_(int fd, void* buf, size_t count);
ssize_t write_(int fd, void* buf, size_t count);
ssize_t read_until(int fd, void* buf, size_t count, char delimiter);
int spawn(const char* file, char* const argv []);


struct execargs_t {
	size_t kol;
	char** args;
};

struct execargs_t* execargs_new(char* str, size_t kol);
void execargs_free(struct execargs_t* ea, int kol);
int exec(struct execargs_t* args);
int runpiped(struct execargs_t** programs, size_t n);


#endif
