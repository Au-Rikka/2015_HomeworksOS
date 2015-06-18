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
    char str[BUF_SIZE + 10];

    struct execargs_t* progs[1000];
    
    int bytes_read;
    int i, j, res;
    int st = 0;
    int en = 0;
    int kol = 0;
    char c;

    while (1) {
        //can't read properly cose i'm stupid, that's why
        write_(STDOUT_FILENO, "$", 1);

        st = 0;
        en = 0;
        kol = 0;
        
        bytes_read = 0;
        do {
            read_(STDIN_FILENO, str + bytes_read, 1);
            bytes_read++;
        } while (str[bytes_read - 1] != '\n');
        str[bytes_read - 1] = ' ';
        bytes_read--;

        /*bytes_read = buf_getline(STDIN_FILENO, buffer, str);

        if (bytes_read < 0) {
            return EXIT_FAILURE;
        }

        if (bytes_read == 0) {
            return EXIT_SUCCESS;
        }
*/

        printf("%s\n", "gonna make an array");

        for (i = 0; i <= bytes_read; i++) {
            if (i == bytes_read || str[i] == '|') {
                en = i;
                if (i < bytes_read) {
                    str[i] = ' ';
                    en++;
                }

              //  printf("kol -> %d\n", en - st);
        
                progs[kol] = execargs_new(str + st, en - st);
               /* 
                printf("%d\n", progs[kol]->kol);
                for (j = 0; j <= progs[kol]->kol; j++) {
                    printf("'%s'\n", progs[kol]->args[j]);
                } 
*/
                if (progs[kol] == NULL) {
                    return EXIT_FAILURE;
                }
                kol++;
                st = i + 1;
            }
        }

        printf("%s\n", "array is ready, gonna run pipe");
      
        res = runpiped(progs, kol);

        if (res == -1) {
            printf("%s\n", "something gone wrong");
            return EXIT_FAILURE;
        }

        for (i = 0; i < kol; i++) {
            execargs_free(progs[i], progs[i]->kol);
        }
        kol = 0;

        printf("%s\n", "everything is fine");
    }

    buf_free(buffer);

    return EXIT_SUCCESS;
}