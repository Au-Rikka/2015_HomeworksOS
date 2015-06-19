#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include "helpers.h"
#include "bufio.h"


#define LISTEN_BACKLOG 100

size_t const BUF_SIZE = 4096;

int make_socket_and_listen(char* port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));

    int q, socket_fd;

    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    q = getaddrinfo(NULL, port, &hints, &result);
    if (q != 0) {
        perror("Could not getaddrinfo\n");
        return EXIT_FAILURE;
    }

    /*  
        getaddrinfo() вернетсписок адресных структур.
        Будем пробовать их по очереди, пока удачно не сбиндимся.
        Если socket или bind падает, закрываем сокет и пробуем следующий адрес из списка.
    */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket_fd == -1) {
            continue;
        }
        if (bind(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;                  /* Success */
        }
        close(socket_fd);
    }

    if (rp == NULL) {               /* No address succeeded */
        perror("Could not bind\n");
        return EXIT_FAILURE;
    }

    freeaddrinfo(result); /* I don't need you anymore T > T   */

    if (listen(socket_fd, LISTEN_BACKLOG) == -1) {
        perror("Could not listen\n");
        return EXIT_FAILURE;
    }

    return socket_fd;
}

int talking(int socket_fd1, int socket_fd2, int fd1, int fd2) {
    pid_t pid;
    pid = fork();
    if (pid == -1) {
        perror("Could not fork\n");
        return EXIT_FAILURE;   
    }

    if (pid == 0) {
        //CHILD
        close(socket_fd1);
        close(socket_fd2);

        struct buf_t* my_buf = buf_new(BUF_SIZE);
        if (my_buf == NULL) {
            perror("Could not make buffer");
            return EXIT_FAILURE;
        }

        int kol;
        while (1) {
            kol = buf_fill(fd1, my_buf, 1);
            if (kol <= 0) {
                break;
            }

            kol = buf_flush(fd2, my_buf, kol);
            if (kol <= 0) {
                break;
            }
        }

        close(fd1);
        close(fd2);
        buf_free(my_buf);

        if (kol == -1) {
            perror("File read/write problems\n");
            return EXIT_FAILURE;       
        }

        return 0;            
    }

    return pid;
}


int main(int argc, char** argv) {
    if (argc < 3) {
        perror("Not enough arguments\n");
        return EXIT_FAILURE;
    }

    char* port1 = argv[1];
    char* port2 = argv[2];


////////////////////////////////////////////////////////////////////////////-SOCKET CREATION

    int socket_fd1 = make_socket_and_listen(port1);
    if (socket_fd1 < 0) {
        return EXIT_FAILURE;
    } 
    int socket_fd2 = make_socket_and_listen(port2);
    if (socket_fd2 < 0) {
        return EXIT_FAILURE;
    }

////////////////////////////////////////////////////////////////////////////-WAITING FOR ACCEPT

    /*
        Теперь можем принимать входящие подключения по одному через accept.
    */

    struct sockaddr_in client1;
    socklen_t client_size1 = sizeof(client1);

    struct sockaddr_in client2;
    socklen_t client_size2 = sizeof(client2);

    while (1) {
        /*
            Извлекается первый запрос на соединение из очереди ожидающих соединений
                для слушающего сокета socket_fd.
            Создается новый подключенный сокет и возвращается файловы дескриптор,
                ссылающийся на сокет.
            Новый сокет не слушает. 
            На socket_fd это никак не влияет.

        */

        int temp_fd1 = accept(socket_fd1, (struct sockaddr*)&client1, &client_size1);
        if (temp_fd1 == -1) {
            perror("Could not accept\n");
            return EXIT_FAILURE;       
        }

        int temp_fd2 = accept(socket_fd2, (struct sockaddr*)&client2, &client_size2);
        if (temp_fd2 == -1) {
            perror("Could not accept\n");
            return EXIT_FAILURE;       
        }


        int res = talking(socket_fd1, socket_fd2, temp_fd1, temp_fd2);
        
        if (res < 0) {
            return EXIT_FAILURE;
        }

        if (res == 0) {
            return EXIT_SUCCESS;
        }
            
        res = talking(socket_fd1, socket_fd2, temp_fd2, temp_fd1);
        
        if (res < 0) {
            return EXIT_FAILURE;
        }

        if (res == 0) {
            return EXIT_SUCCESS;
        }    

        close(temp_fd1);
        close(temp_fd2);
    }

    close(socket_fd1);
    close(socket_fd2);

    return EXIT_SUCCESS;
}