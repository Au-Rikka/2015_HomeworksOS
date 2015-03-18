#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

size_t const BUF_SIZE = 4096;

int main() {
    char* buf = (char*) malloc(BUF_SIZE);
    char rev_buf[BUF_SIZE];
    ssize_t bytes_read = 1;
    ssize_t bytes_write = 0;
    int offset = 0;

    while (bytes_read > 0) {
        bytes_read = read_until(STDIN_FILENO, buf + offset, BUF_SIZE, ' ');
        if (bytes_read < 0) {
            perror("Input error");
            return EXIT_FAILURE;
        }
        if (bytes_read > 0) {
            int i, j;
            int st = 0;
            for (i = offset; i < bytes_read + offset; i++) {
                if (buf[i] == ' ') {
                    for (j = i - 1; j >= st; j--) {
                        rev_buf[i - 1 - j] = buf[j];
                    }
                    rev_buf[i - st] = ' ';
                    bytes_write = write_(STDOUT_FILENO, rev_buf, i - st + 1);
                    if (bytes_write < 0) {
                        perror("Output error");
                        return EXIT_FAILURE;
                    }
                    st = i + 1;
                }
            }
            offset += bytes_read - st;
            buf += st;
        }
    }

    int j;
    for (j = offset; j >= 0; j--) {
        rev_buf[offset - j] = buf[j];
    }
    bytes_write = write_(STDOUT_FILENO, rev_buf, offset + 1);
    if (bytes_write < 0) {
        perror("Output error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
