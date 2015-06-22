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
#include <poll.h>
#include "helpers.h"
#include "bufio.h"
#include <errno.h> 

#define LISTEN_BACKLOG 100
#define BUF_SIZE 4096
#define TIMEOUT -1
#define MAX_N 256

struct pollfd poll_list[MAX_N];
struct buf_t* bufs[MAX_N];
int fd_closed[MAX_N];
nfds_t kol;
int i, j;



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
        getaddrinfo() вернет список адресных структур.
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


int change_poll_list(int i) {
//swap curent pair of sockets with the last one
//i should be the nomber of first socket in pair

    poll_list[i] = poll_list[kol - 1];
    buf_free(bufs[i]);
    bufs[i] = bufs[kol - 1];
    //closed[i] = closed[kol - 1];
                        
    poll_list[i - 1] = poll_list[kol - 2];
    buf_free(bufs[i - 1]);
    bufs[i - 1] = bufs[kol - 2];
    //closed[i - 1] = closed[kol - 2];

    i--;
   
    return i;
}






int main(int argc, char** argv) {
    if (argc < 3) {
        perror("Not enough arguments\n");
        return EXIT_FAILURE;
    }

    char* port1 = argv[1];
    char* port2 = argv[2];

////////////////////////////////////////////////////////////////////////////-SOCKET CREATION

    poll_list[0].fd = make_socket_and_listen(port1);
    if (poll_list[0].fd < 0) {
        return EXIT_FAILURE;
    } 
    poll_list[1].fd = make_socket_and_listen(port2);
    if (poll_list[1].fd < 0) {
        return EXIT_FAILURE;
    }

    poll_list[0].events = POLLIN;
    poll_list[1].events = 0;
    kol = 2;
    printf("printing events %d, %d\n", poll_list[0].events, poll_list[1].events);


////////////////////////////////////////////////////////////////////////////-WAITING FOR ACCEPT    
    printf("gonna wait for all my fucking life\n");

    struct sockaddr_in client[MAX_N];
    socklen_t client_size[MAX_N];
    for (i = 0; i < MAX_N; i++) {
        client_size[i] = sizeof(client[i]);
    }


    while (1) {
        printf("i'm just right befor fucking poll\n");
        printf("printing events %d, %d\n", poll_list[0].events, poll_list[1].events);
        int res = poll(poll_list, kol, TIMEOUT);
        printf("after poll\n");
        printf("printing revents %d, %d\n", poll_list[0].revents, poll_list[1].revents);
        if (res == -1) {
            if (errno != EINTR) {
                perror("Cannot poll");
            }
        } else if (res == 0) {
            perror("Timeout\n");
            return EXIT_FAILURE;
        } else {
            if (kol < MAX_N) {
                if ((poll_list[0].revents & POLLIN) != 0) {
                    //на первый порт что-то пришло
                    printf("first port has something\n");
                    poll_list[kol].fd = accept(poll_list[0].fd, (struct sockaddr*)&client[kol], &client_size[kol]);
                    if (poll_list[kol].fd == -1) {
                        perror("Could not accept\n");
                        return EXIT_FAILURE;
                    }
                    kol++;
                    //закрыть первый порт, открыть второй
                    poll_list[0].events = 0;
                    poll_list[1].events = POLLIN;
                }

                if ((poll_list[1].revents & POLLIN) != 0) {
                    //на второй порт что-то пришло
                    printf("second port has something\n");
                    poll_list[kol].fd = accept(poll_list[1].fd, (struct sockaddr*)&client[kol], &client_size[kol]);
                    printf("i'm right after 2 accept\n");
                    if (poll_list[kol].fd == -1) {
                        perror("Could not accept\n");
                        return EXIT_FAILURE;
                    }
                    kol++;
                    //закрыть второй порт, открыть первый
                    poll_list[0].events = POLLIN;
                    poll_list[1].events = 0;
                    //подготовить пару сокетов к обмену данными
                    poll_list[kol - 2].events = POLLIN;
                    poll_list[kol - 1].events = POLLIN;
                    bufs[kol - 2] = buf_new(BUF_SIZE);
                    bufs[kol - 1] = buf_new(BUF_SIZE);
     //               fd_closed[kol - 2] = 0;
      //              fd_closed[kol - 1] = 0;
                    printf("i've made some nice sockets\n");
                } 
            } else {
                //все занято, так что просто
                //сбросить ожидаемые события портов
                poll_list[0].events = 0;
                poll_list[1].events = 0;
            }

            //обходим остальные сокеты
            i = 2;
            while (i < kol) {
                int j;
                if (i % 2 == 1) {
                    j = i + 1;
                } else {
                    j = i - 1;
                }

                printf("checking for %d port\n", i);

                
                if (poll_list[i].revents & POLLIN) {
                    //можно прочитать что-нибудь из i
                    int res = buf_fill(poll_list[i].fd, bufs[i], 1);

                    if (res <= 0) {
                        poll_list[i].events &= (~POLLIN);
                        
                        if (bufs[i]->size == 0) {
                            poll_list[j].events &= (~POLLOUT);
                        }
                    } else {
/*
                    if (res <= 0) {
                        //если ничего не считалось
                        //вырубить чтение в i
                        shutdown(poll_list[i].fd, SHUT_RD);
                        fd_closed[i] = fd_closed[i] | 1;
                        poll_list[i].events = poll_list[i].events & (~POLLIN);
            
                        //если буфер еще и пуст, вырубить запись в j
                        if (poll_list[i].size == 0 || res == -1) {
                            shutdown(poll_list[j].fd, SHUT_WR);
                            fd_closed[j] = fd_closed[j] | 2;
                            poll_list[j].events = poll_list[j].events & (~POLLOUT);
                        }
                    } else {
                        //что-то считалось, включить ожидание записи в j
                        poll_list[j].events = poll_list[j].events | POLLOUT;
                    }
  */                       

                        if (bufs[i]->size == bufs[i]->capacity) {
                            poll_list[i].events &= (~POLLIN);
                        }
                        if (bufs[i]->size > 0) {
                            poll_list[i].events |= POLLOUT;
                        }
                    }
                }


                if (poll_list[i].revents & POLLOUT) {
                    //можно записать что-нибудь в i
                    int res = buf_flush(poll_list[i].fd, bufs[j], 1);

                    if (res <= 0) {
                    //        shutdown(poll_list[i].fd, SHUT_WR);
                    //  нужен, если ловить POLLHUP
                    //        fd_closed[i] = fd_closed[i] | 2;
                    //  неебу, нужна ли вообще эта фигня

                        poll_list[i].events &= (~POLLOUT);
                        if (res == -1 || bufs[j]->size != 0) {
                            poll_list[j].events &= (~POLLOUT);
                        }
                    } else {
                    
    /*                if (res <= 0) {
                        //ничего не вывелось
                        if (bufs[j].size > 0 || res == -1) {
                            //вырубить запись в i
                            shutdown(poll_list[i].fd, SHUT_WR);
                            fd_closed[i] = fd_closed[i] | 2;
                            poll_list[i].events = poll_list[i].events & (~POLLOUT);
                            //чтение в j, кстати, тоже больше не нужно
                            shutdown(poll_list[j].fd, SHUT_RD);
                            fd_closed[j] = fd_closed[j] | 1;
                            poll_list[j].events = poll_list[j].events & (~POLLIN);
                        } else {
                            if ((fd_closed[j] & 1) == 1) {

                            }
                        }

                    if (res <= 0) {
                        poll_list[j].events = poll_list[j].events & (!POLLOUT);
                    } else {
                        poll_list[j].events = poll_list[j].events | POLLOUT;
                    }
*/
                        if (bufs[j]->size < bufs[j]->capacity) {
                            poll_list[j].events |= POLLIN;
                        }
                        if (bufs[j]->size == 0) {
                            poll_list[j].events &= (~POLLOUT);
                        }
                    }
                }

                int sum = (poll_list[i].events & POLLOUT) + (poll_list[i].events & POLLIN); 
                sum += (poll_list[j].events & POLLOUT) + (poll_list[j].events & POLLIN); 
                if (sum == 0) {
                    //пара больше не нужна, выкинем ее из списка
                    if (i % 2 == 0) {
                        i = change_poll_list(i);
                    } else {
                        i = change_poll_list(j);
                    }
                    kol -= 2;
                } else {
                    i++;
                }
            }
        }
    }

    close(poll_list[0].fd);
    close(poll_list[1].fd);

    return EXIT_SUCCESS;
}