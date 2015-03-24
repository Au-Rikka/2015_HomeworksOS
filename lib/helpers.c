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
