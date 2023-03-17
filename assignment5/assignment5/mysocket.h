#ifndef __MY_SOCKET
#define __MY_SOCKET

/**
 * NOTE: CAN I LIMIT ACCESS USING A COMMANDLINE OPEARTED MACRO?
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SOCK_MyTCP 69

/**
 * socket alternative
 * 
 * This function opens a standard TCP socket with the socket call. It also 
 * creates two threads R and S, allocates and initializes space for 
 * two tables Send_Message and Received_Message, and any 
 * additional space that may be needed. The parameters are the same as the 
 * normal socket( ) call, but it will take only SOCK_MyTCP as the socket type
 * 
 * 
 * @param domain [in] the protocol still the internet type (int)
 * @param type [in] the socket type, only SOCK_MyTYPE (int), or ERROR
 * @param protocol [in] same as socket()
 * 
 * @return socket, returns the socket
*/

int my_socket(int domain, int type, int protocol);

int my_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int my_listen(int sockfd, int backlog);

int my_accept(int sockfd, struct sockaddr *cliaddr, socklen_t *clilen);

int my_connect(int sockfd, struct sockaddr *addr, socklen_t addrlen);

int my_recv(int sockfd, void *buf, size_t len, int flags);

int my_send(int sockfd, const void *buf, size_t len, int flags);

void my_close(int sockfd);

#endif