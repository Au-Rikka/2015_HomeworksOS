#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"

size_t const BUF_SIZE = 4097;

int main() {
    char* buf = (char*) malloc(BUF_SIZE);
    char* rev_buf = (char*) malloc(BUF_SIZE);
 
    if (buf == NULL || rev_buf == NULL) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    ssize_t bytes_read = 0;
    ssize_t bytes_write = 0;
    int offset = 0;
    int ri = 0;

    while (1) {
        bytes_read = read_until(STDIN_FILENO, buf + offset, BUF_SIZE, ' ');
        
        if (bytes_read < 0) {
            perror("Input error");
            return EXIT_FAILURE;
        }
        if (bytes_read == 0) {
            break;
        }
        
        int i, j;
        int kol = 0;
        for (i = offset; i < bytes_read + offset; i++) {
            if (buf[i] == ' ') {
                for (j = i - 1; j >= kol; j--) {
                    rev_buf[ri] = buf[j];
                    ri++;
                }

                rev_buf[ri] = ' ';
                ri++;

                bytes_write = write_(STDOUT_FILENO, rev_buf, ri);
                if (bytes_write < 0) {
                    perror("Output error");
                    return EXIT_FAILURE;
                } 

                ri -= bytes_write;
                memmove(rev_buf, rev_buf + bytes_write, ri);
                kol += bytes_write;
            }
        }

        offset += bytes_read - kol;
        memmove(buf, buf + kol, offset);
    }

    int j;
    for (j = offset - 1; j >= 0; j--) {
        rev_buf[ri] = buf[j];
        ri++;
    }
    bytes_write = write_(STDOUT_FILENO, rev_buf, ri);
    if (bytes_write < 0) {
        perror("Output error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
