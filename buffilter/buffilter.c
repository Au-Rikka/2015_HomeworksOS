#include <stdio.h>
#include <stdlib.h>
#include "bufio.h"

size_t const BUF_SIZE = 4096;

int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        perror("not enought arguments");
        return EXIT_FAILURE;
    }

    struct buf_t* super_buffer = buf_new(BUF_SIZE);    
    struct buf_t* super_write_buffer = buf_new(BUF_SIZE);
    if (super_buffer == NULL || super_write_buffer == NULL) {
        perror("buffer creation failed");
        return EXIT_FAILURE;
    }

    char* args[argc + 1];
    
    ssize_t bytes_read = 0;
    int i;
    int k = 0;
    int kol = 0;


    for (i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }
    args[argc] = NULL;  

    while (1) {
        char* buf = (char*) malloc(BUF_SIZE);
        if (buf == NULL) {
            perror("malloc failed");
            return EXIT_FAILURE;   
        }
        bytes_read = buf_getline(STDIN_FILENO, super_buffer, buf);

        if (bytes_read <= 0) {
            break;
        }

        buf[bytes_read] = '\0';
        args[argc - 1] = buf;
        if (spawn(argv[1], args) == 0) {
            buf[bytes_read] = '\n';
            if (buf_write(STDOUT_FILENO, super_write_buffer, buf, bytes_read + 1) == -1) {
                perror("output fail");
                return EXIT_FAILURE;
            }
        }
        bytes_read = 0;

        free(buf);
    }

    if (bytes_read < 0) {
        perror("Input error");
        return EXIT_FAILURE;
    }    

    if (buf_flush(STDOUT_FILENO, super_write_buffer, super_write_buffer->size) < 0) {
        perror("output fail");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}