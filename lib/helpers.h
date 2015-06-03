#ifndef HELPERS_H

#define HELPERS_H

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

ssize_t read_(int fd, void* buf, size_t count);
ssize_t write_(int fd, void* buf, size_t count);
ssize_t read_until(int fd, void* buf, size_t count, char delimiter);
int spawn(const char* file, char* const argv []);

/*
struct exacargs_t {
	size_t kol;
	char** args;
};

struct exacargs_t* exacargs_new(char** args, size_t kol);
void exacargs_free(struct exacargs_t* ea);
int exec(execargs_t* args);
int runpiped(execargs_t** programs, size_t n);
*/


#endif
