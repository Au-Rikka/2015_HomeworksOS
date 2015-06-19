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


int main(int argc, char** argv) {
    if (argc < 3) {
        perror("Not enough arguments\n");
        return EXIT_FAILURE;
    }

    char* port = argv[1];
    char* file_name = argv[2];

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));

    int q, socket_fd;

////////////////////////////////////////////////////////////////////////////-SOCKET CREATION

    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
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
    
////////////////////////////////////////////////////////////////////////////-WAITING FOR ACCEPT

    /*
        Теперь можем принимать входящие подключения по одному через accept.
    */

    struct sockaddr_in client;
    socklen_t client_size = sizeof(client);

    while (1) {
        /*
            Извлекается первый запрос на соединение из очереди ожидающих соединений
                для слушающего сокета socket_fd.
            Создается новый подключенный сокет и возвращается файловы дескриптор,
                ссылающийся на сокет.
            Новый сокет не слушает. 
            На socket_fd это никак не влияет.

        */

        int temp_fd = accept(socket_fd, (struct sockaddr*)&client, &client_size);
        
        if (temp_fd == -1) {
            perror("Could not accept\n");
            return EXIT_FAILURE;       
        }

        pid_t pid;
        pid = fork();
        if (pid == -1) {
            perror("Could not fork\n");
            return EXIT_FAILURE;   
        }

        if (pid == 0) {
            //CHILD
            close(socket_fd);

            int file_fd = open(file_name, 0);
            if (file_fd == -1) {
                perror("Could not open file\n");
                return EXIT_FAILURE;       
            }

            struct buf_t* my_buf = buf_new(BUF_SIZE);

            if (my_buf == NULL) {
                perror("Could not make buffer");
                return EXIT_FAILURE;
            }

            int kol;
            while (1) {
                kol = buf_fill(file_fd, my_buf, BUF_SIZE);
                if (kol <= 0) {
                    break;
                }

                kol = buf_flush(temp_fd, my_buf, BUF_SIZE);
                if (kol <= 0) {
                    break;
                }
            }

            close(temp_fd);
            buf_free(my_buf);
            
            if (kol == -1) {
                perror("File read/write problems\n");
                return EXIT_FAILURE;       
            }

            return EXIT_SUCCESS;            
        }

        close(temp_fd);
    }

    close(socket_fd);

    return EXIT_SUCCESS;
}