#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

size_t const BUF_SIZE = 4096;

int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        perror("not enought arguments");
        return EXIT_FAILURE;
    }

    char* buf = (char*) malloc(BUF_SIZE);
    char* param = (char*) malloc(BUF_SIZE);
    char* args[argc + 1];
    ssize_t bytes_read = 0;
    int i;
    int k = 0;


    for (i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }
    args[argc] = NULL;  

    bytes_read = read_until(STDIN_FILENO, buf, BUF_SIZE - k, '\n');
    while (bytes_read > 0) {
        for (i = 0; i < bytes_read; i++) {
            if (buf[i] == '\n') {
                param[k] = '\0';
                args[argc - 1] = param;
                if (spawn(argv[1], args) == 0) {
                    param[k] = '\n';
                    if (write_(STDOUT_FILENO, param, k + 1) == -1) {
                        perror("output fail");
                        return EXIT_FAILURE;
                    }
                }
                k = 0;
            } else {
                param[k] = buf[i];
                k++;
            }
        }
            
        buf += bytes_read;

        bytes_read = read_until(STDIN_FILENO, buf, BUF_SIZE - k, '\n');
    }

    if (bytes_read < 0) {
        perror("Input error");
        return EXIT_FAILURE;
    }

    if (k != 0) {
        param[k] = '\0';
        args[argc - 1] = param;
        if (spawn(argv[1], args) == 0) {
            param[k] = '\n';
            if (write_(STDOUT_FILENO, param, k + 1) == -1) {
                perror("output fail");
                return EXIT_FAILURE;
            }
        }
                    
    }

    return EXIT_SUCCESS;
}