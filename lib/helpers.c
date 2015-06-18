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


///////////////////////////////////////////////////////////////////////


void execargs_free(struct execargs_t* ea, int kol) {
    #ifdef DEBUG
        if (ea == NULL) {
            abort();
        }
    #endif

    int i;
    for (i = 0; i < kol; i++) {
        free(ea->args[i]);
    }

    free(ea->args);
    free(ea);
}


struct execargs_t* execargs_new(char* str, size_t kol) {
    int size = 0;
    int i;

    for (i = 0; i < kol; i++) {
        if (str[i] == ' ' && i > 0 && str[i - 1] != ' ') {
            size++;
        }
      //  printf("%c", str[i]);
    }
    //printf("\n");
    if (str[kol - 1] != ' ') {
        size++;
    }
    //printf("!!size->%d\n", size);

    if (size == 0) {
        return NULL;
    }

    struct execargs_t* ea = (struct execargs_t*) malloc(sizeof(struct execargs_t));
    if (ea == NULL) {
        return NULL;
    }

    ea->args = (char**) malloc((size + 1) * sizeof(char*));
    if (ea == NULL) {
        return NULL;
    }
    ea->kol = size;
   // printf("size = %zu\n", size);
    
    int j = 0;
    int x = -1;
    for (i = 0; i < kol; i++) {
        while (i < kol && str[i] == ' ') {
            i++;
        }
        if (i < kol) {
            x = i;
        } else {
            x = -1;
            break;
        }

        while (i < kol && str[i] != ' ') {
           // printf("'%c'\n", str[i]);
            i++;
        }
        str[i] = 0;
        if (x != -1) {
            ea->args[j] = strdup(str + x);
         //   printf("it's i lol -.>     %d\n", i);
            j++;
        }
    }    

    ea->args[size] = NULL;

    return ea;
}


int exec(struct execargs_t* args) {
//    printf ("hah -> '%s'\n", args->args[0]);
 //   if (args->args[1] == NULL) printf("ok\n");
    int res = execvp(args->args[0], args->args);
   // printf("%s\n", "exec");
    return res;
}

int runpiped(struct execargs_t** programs, size_t n) {
    int pipefd[n][2];
    int res;
    int i, j;

    for (i = 0; i < n - 1; i++) {
        res = pipe(pipefd[i]);

        if (res == -1) {
            return -1;
        }
    }

    for (i = 0; i < n; i++) {
        pid_t p = fork();

        if (p == -1) {
            perror("Cannot fork");
            return -1;
        }

        if (p == 0) {
            printf("%s %d %s %d\n", "i'm in child of", i, "process of ", n);
            //child
            //проверить пайп, если закрыт - вернуть 0
            if (i != 0) {
                //int dup2(int oldfd, int newfd);
                //dup2 делает newfd копией oldfd, закрывая newfd, если потребуется
                //возвращает -1 при ошибке

                close(STDIN_FILENO);
                dup2(pipefd[i - 1][0], STDIN_FILENO);
            }

            if (i != n - 1) {
                close(STDOUT_FILENO);
                dup2(pipefd[i][1], STDOUT_FILENO);
            }

            for (j = 0; j < n - 1; j++) {
                if (j != i - 1) {
                    close(pipefd[j][0]);
                } 
                if (j != i) {
                    close(pipefd[j][1]);
                }
            }

            res = exec(programs[i]);
            
          //  close(pipefd[i - 1][0]);
          //  close(pipefd[i][0]);

            return res;
        }

        if (i != 0) {
            close(pipefd[i - 1][0]);
        }
        if ( i != n - 1) {
            close(pipefd[i][1]);
        }
    
        //parent
    }

    int status;
    for (i = 0; i < n; i++) {
        if (status == -1) {
            perror("Cannot wait");
            return -1;
        }

        if (!WIFEXITED(status)) {
            perror("exit error");
            return -1;
        }
    }

    for (i = 0; i < n - 1; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }


    return 0;
}

