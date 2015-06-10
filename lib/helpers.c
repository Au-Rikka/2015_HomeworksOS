#include "helpers.h"

ssize_t read_(int fd, void* buf, size_t count) {
    ssize_t bytes_read = 0;
    ssize_t cur = 1;

    while (cur > 0 && count > 0) {
        cur = read(fd, buf + bytes_read, count);
        if (cur < 0) {
            return cur;
        }
        bytes_read += cur;
        count -= cur;
    }

    return bytes_read;
}


ssize_t write_(int fd, void* buf, size_t count) {
    ssize_t bytes_write = 0;
    ssize_t cur = 1;

    while (cur > 0 && count > 0) {
        cur = write(fd, buf + bytes_write, count);
        if (cur < 0) {
            return cur;
        }
        bytes_write += cur;
        count -= cur;
    }

    return bytes_write;
}

ssize_t read_until(int fd, void* buf, size_t count, char delimiter) {
    ssize_t bytes_read = 0;
    ssize_t cur = 1;
    char* chars = (char*) buf;

    while (cur > 0 && count > 0) {
        cur = read(fd, buf + bytes_read, count);
        if (cur < 0) {
            return cur;
        }
        
        int i;
        for (i = bytes_read; i < bytes_read + cur; i++) {
            if (chars[i] == delimiter) {
                return bytes_read + cur;
            }
        }
        bytes_read += cur;
        count -= cur;
    }

    return bytes_read;
}



int spawn(const char * file, char * const argv []) {
    pid_t p = fork();
    if (p == -1) {
        perror("Cannot fork");
        return -1;
    }

    if (p == 0) {
        //child
        return execvp(file, argv);
    }
    
    //parent
    int status;
    wait(&status);
    if (status == -1) {
        perror("Cannot wait");
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        perror("exit error");
        return -1;
    }
}


///////////////////////////////////////////////////////////////////////


void execargs_free(struct execargs_t* ea, int kol) {
    #ifdef DEBUG
        if (ea == NULL) {
            abort();
        }
    #endif

    int i;
    for (i = 0; i < kol; i++) {
        free(ea->args[i]);
    }

    free(ea->args);
    free(ea);
}


struct execargs_t* execargs_new(char** args, size_t kol) {
    struct execargs_t* ea = (struct execargs_t*) malloc(sizeof(struct execargs_t));
    if (ea == NULL) {
        return NULL;
    }

    ea->args = (char**) malloc((kol + 1) * sizeof(char*));
    if (ea == NULL) {
        return NULL;
    }
    ea->kol = kol;

    size_t i;
    for (i = 0; i < kol; i++) {
        ea->args[i] = args[i]; //strdup(args[i])
        if (ea->args[i] == NULL) {
            execargs_free(ea, i);
            return NULL;
        }
    }    
    ea->args[kol] = NULL;

    return ea;
}


int exec(struct execargs_t* args) {
    pid_t p = fork();

    if (p == -1) {
        perror("Cannot fork");
        return -1;
    }

    if (p == 0) {
        //child
        return execvp(args->args[0], args->args);
    }
    
    //parent
    int status;
    wait(&status);
    if (status == -1) {
        perror("Cannot wait");
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        perror("exit error");
        return -1;
    }
}

int runpiped(struct execargs_t** programs, size_t n) {
    return 1;
}

