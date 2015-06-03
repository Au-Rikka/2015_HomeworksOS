#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

size_t const BUF_SIZE = 5000;

int main() {
    char* buf = (char*) malloc(BUF_SIZE);
    char* rev_buf = (char*) malloc(BUF_SIZE);

    int bytes_read = 1;
    int bytes_write = 0;
    int offset = 0;
    int st = 0; //first not copied element in buf
    int en = 0; //first free element in rev_buf 

    while (bytes_read > 0) {
        bytes_read = read_until(STDIN_FILENO, buf + offset, BUF_SIZE, ' ');
        if (bytes_read < 0) {
            perror("Input error");
            return EXIT_FAILURE;
        }

        if (bytes_read > 0) {
            int i, j;
            for (i = offset; i < bytes_read + offset; i++) {
                if (buf[i] == ' ') {
                    for (j = i - 1; j >= st; j--) {
                        rev_buf[en] = buf[j];
                        en++;
                        if (en > BUF_SIZE) {
                            perror("Output error");
                            return EXIT_FAILURE;
                        }
                    }
                    rev_buf[en] = ' ';
                    en++;
                    if (en > BUF_SIZE) {
                        perror("Output error");
                        return EXIT_FAILURE;
                    }

                    bytes_write = write_(STDOUT_FILENO, rev_buf, en);
                    if (bytes_write < 0) {
                        perror("Output error");
                        return EXIT_FAILURE;
                    }

                    memcpy(rev_buf, rev_buf + bytes_write, en - bytes_write);
                    en = en - bytes_write;
                    st = i + 1;
                }
            }

            offset = offset + bytes_read; 
            memcpy(buf, buf + st, offset - st);
            offset = offset - st;
            st = 0;
        }
    }

    int j;
    for (j = offset - 1; j >= 0; j--) {
        rev_buf[en] = buf[j];
        en++;
        if (en > BUF_SIZE) {
            perror("Output error");
            return EXIT_FAILURE;
        }
    }

    bytes_write = write_(STDOUT_FILENO, rev_buf, en);
    if (bytes_write < 0) {
        perror("Output error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
