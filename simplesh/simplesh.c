#include "helpers.h"
#include "bufio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t const BUF_SIZE = 4096;
int main(int argc, char *argv[]) {

    struct sigaction sa, old_sa;   
    sa.sa_flags = 0; //флаги, которые влияют на поведение процесса
    sigemptyset(&sa.sa_mask); // маска сигналов, блокируемых при обработке
    sa.sa_handler = SIG_IGN; //DIG_DFL, SIG_IGN, или указатель на функцию
    
    sigaction(SIGINT, &sa, &old_sa);


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
            sigaction(SIGINT, &old_sa, NULL);
            buf_free(buffer);
            return EXIT_FAILURE;
        }

        if (bytes_read == 0) {
            sigaction(SIGINT, &old_sa, NULL);
            buf_free(buffer);
            return EXIT_SUCCESS;
        }

        if (str[bytes_read - 1] != '\n') {
            perror("line is too long");
            sigaction(SIGINT, &old_sa, NULL);
            buf_free(buffer);
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

                progs[kol] = execargs_new(str + st, en - st);
                if (progs[kol] == NULL) {
                    for (j = 0; j < kol; j++) {
                        execargs_free(progs[j], progs[j]->kol + 2);
                    }
                    kol = -1;
                    break;
                }
                st = en;
                kol++;
            }
        }

        if (kol > 0) {
            printf("%s\n", "array is ready, gonna run pipe");
      
            res = runpiped(progs, kol);

            if (res == -1) {
                printf("%s\n", "something gone wrong");
            }

            for (i = 0; i < kol; i++) {
                execargs_free(progs[i], progs[i]->kol + 2);
            }
        }
        
        

        break;
        printf("%s\n", "everything is fine");
    }

    sigaction(SIGINT, &old_sa, NULL);
    buf_free(buffer);


    return EXIT_SUCCESS;
}