#include "helpers.h"
#include "bufio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t const BUF_SIZE = 4096;
int main(int argc, char *argv[]) {

    struct buf_t* buffer = buf_new(BUF_SIZE);
    if (buffer == NULL) {
        return EXIT_FAILURE;
    }

    char str[BUF_SIZE * 2];
    int bytes_read;
    struct execargs_t* progs[1000];
    int i, j, res;
    
    while (1) {
        write_(STDOUT_FILENO, "$", 1);         
        bytes_read = buf_getline(STDIN_FILENO, buffer, str);
        printf("'%s'\n", str);

        if (bytes_read < 0) {
            perror("input error");
            return EXIT_FAILURE;
        }

        if (bytes_read == 0) {
            return EXIT_SUCCESS;
        }

        if (str[bytes_read - 1] != '\n') {
            perror("line is too long");
            return EXIT_FAILURE;
        }

        printf("%s\n", "gonna make an array");
        int st = 0;
        int en = 0;
        int kol = 0;
        for (i = 0; i < bytes_read; i++) {
            if (str[i] == '\n' || str[i] == '|') {
                en = i + 1;
                str[i] = ' ';       

                printf("%d, %d\n", st, en);
                progs[kol] = execargs_new(str + st, en - st);
                if (progs[kol] == NULL) {
                    return EXIT_FAILURE;
                }
                st = en;
                kol++;
            }
        }

        printf("%s\n", "array is ready, gonna run pipe");
      
        res = runpiped(progs, kol);

        if (res == -1) {
            printf("%s\n", "something gone wrong");
        }

        for (i = 0; i < kol; i++) {
            execargs_free(progs[i], progs[i]->kol);
        }

        printf("%s\n", "everything is fine");
    }

    buf_free(buffer);
    return EXIT_SUCCESS;
}