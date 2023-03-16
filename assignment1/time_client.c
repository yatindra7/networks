#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//CONSTANTS
#define SERVER_PORT htons(2001)
#define SERVER_IP "127.0.0.1"
#define RECV_BUF_SIZE 100
#define SEND_BUF_SIZE 100

// ERROR HANDLING MACROS
#define SOCK_CREATE_ERR do {perror("[ERROR] UNABLE TO OPEN SOCKET "); exit(EXIT_FAILURE);} while (0)
#define SERVER_CONN_ERR do {perror("[ERROR] UNABLE TO CONNECT TO SERVER "); exit(EXIT_FAILURE);} while (0)

// INITIALIZE SERVER ADDR
#define INIT_SERV_ADDR(SERV_ADDR) do { \
                                        SERV_ADDR.sin_family = AF_INET; \
                                        inet_aton(SERVER_IP, &SERV_ADDR.sin_addr); \
                                        SERV_ADDR.sin_port = SERVER_PORT; \
                                    } while (0)

// CONNECT MACRO
#define CONNECT(SOCK_FD,SERV_ADDR) do { \
                                        int conn = connect(SOCK_FD, (struct sockaddr *) &SERV_ADDR, sizeof(SERV_ADDR)); \
                                        if (conn < 0) SERVER_CONN_ERR; \
                                    } while (0)

// SEND-RECV MACROS
#define SEND(SOCK_FD,MSG) send(SOCK_FD, MSG, strlen(MSG) + 1, 0)
#define RECV(SOCK_FD,BUF) recv(SOCK_FD, BUF, RECV_BUF_SIZE, 0)

// PRINT DATETIME
void print_datetime (char *datetime) {
    printf("DATETIME RECEIVED: %s\n", datetime);
    return;
}

int main () {
    // SOCKET DESCRIPTOR
    int sock_fd, bytes_recvd;

    // BUFFERS
    char buf[RECV_BUF_SIZE];
    memset(buf, 0, sizeof(buf));

    // ADDRESS
    struct sockaddr_in serv_addr;

    // CREATING SOCKET
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) SOCK_CREATE_ERR;

    // CREATING THE SERVER ADDR STRUCT
    INIT_SERV_ADDR(serv_addr);

    // CONNECTING TO THE SERVER
    CONNECT(sock_fd, serv_addr);

    while (1) {
        bytes_recvd = RECV(sock_fd, buf);
        if (buf[bytes_recvd] == '\0') {
            print_datetime(buf);
            exit(EXIT_SUCCESS);
        }
    }
}