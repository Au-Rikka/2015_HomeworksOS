#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

size_t const BUF_SIZE = 2048;

int main() {
    char buf[BUF_SIZE];
    ssize_t bytes_read = 1;
    ssize_t bytes_write = 0;

    while (bytes_read > 0) {
        bytes_read = read_(STDIN_FILENO, buf, BUF_SIZE);
        if (bytes_read < 0) {
            perror("Input error");
            return EXIT_FAILURE;
        }
        if (bytes_read > 0) {
            bytes_write = write_(STDOUT_FILENO, buf, bytes_read);
            if (bytes_write < 0) {
                perror("Output error");
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}
