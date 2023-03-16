#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//NUMERICAL CONSTANTS
#define SERVER_PORT htons(2001)
#define RECV_BUF_SIZE 100
#define SEND_BUF_SIZE 100
#define BACKLOG 5

// ERROR HANDLING MACROS
#define SOCK_CREATE_ERR do { \
                            perror("[ERROR] UNABLE TO OPEN SOCKET "); \
                            exit(EXIT_FAILURE); \
                            } while (0)
#define BIND_ERR do { \
                        perror("[ERROR] UNABLE TO BIND TO LOCAL ADDR "); \
                        exit(EXIT_FAILURE); \
                        } while (0)
#define CONN_ACCEPT_ERR do { \
                        perror("[ERROR] UNABLE TO ACCEPT CONNECTION "); \
                        exit(EXIT_FAILURE); \
                        } while (0)

// SEND-RECV MACROS
#define SEND(SOCK_FD,MSG) do { \
                                int __bytes_sent = 0; \
                                while (1) { \
                                    __bytes_sent += send(SOCK_FD, (MSG) + __bytes_sent, (strlen(MSG) + 1) - __bytes_sent, 0); \
                                    if (__bytes_sent == (strlen(MSG) + 1)) break; \
                                } \
                            } while (0)

#define RECV(SOCK_FD,BUF) recv(SOCK_FD, BUF, RECV_BUF_SIZE, 0)

// INITIALIZE SERVER ADDR
#define INIT_SERV_ADDR(SERV_ADDR) do { \
                                        SERV_ADDR.sin_family = AF_INET; \
                                        SERV_ADDR.sin_addr.s_addr = INADDR_ANY; \
                                        SERV_ADDR.sin_port = SERVER_PORT; \
                                    } while (0)

// BIND MACRO
#define BIND(SOCK_FD,SERV_ADDR_IN) do { \
                                        int bind_res = bind(SOCK_FD, (struct sockaddr *) &SERV_ADDR_IN, sizeof(SERV_ADDR_IN)); \
                                        if (bind_res < 0) BIND_ERR; \
                                    } while (0)

// ACCEPT MACRO
#define ACCEPT(SOCK_FD,CLI_ADDR,CONN_SOCK_FD) do { \
                                                    socklen_t cli_len = sizeof(CLI_ADDR); \
                                                    CONN_SOCK_FD = accept(SOCK_FD, (struct sockaddr *) &CLI_ADDR, &cli_len); \
                                                    if (CONN_SOCK_FD < 0) CONN_ACCEPT_ERR; \
                                                } while (0)

int get_datetime(char *buf) {
    time_t t = time(NULL); struct tm tm = *localtime(&t);
    return sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// PRINT DATETIME
void print_datetime (char *datetime) {
    printf("[LOG] DATETIME TO BE SENT: %s\n", datetime);
    return;
}

int main () {
    // THE DESCRIPTORS
    int sock_fd, conn_sock_fd;

    // THE SOCKET ADDRESSES
    struct sockaddr_in serv_addr, cli_addr;

    // BUFFER TO SEND DATE AND TIME
    char datetime[SEND_BUF_SIZE];
    memset(datetime, 0, sizeof(datetime));

    // CREATING SOCKET
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) SOCK_CREATE_ERR;

    // CREATING STRUCT OF ADDRESSES
    INIT_SERV_ADDR(serv_addr);

    // BINDING TO THE ADDRESS
    BIND(sock_fd, serv_addr);

    // LISTENING
    listen(sock_fd, BACKLOG);

    printf("[LOG] SERVER READY TO ACCEPT CONNECTIONS @%s:%d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
    while (1) {
        printf("[LOG] SERVER IDLE< WAITING FOR CONNECTIONS\n");

        // ACCEPTING THE CONNECTION AND SAVING SOCKET DESC. IN conn_sock_fd
        ACCEPT(sock_fd, cli_addr, conn_sock_fd);

        // CLIENT CONNECTTED
        printf("[LOG] CONNECT> %s, CONNECTED@PORT %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // STORING DATE-TIME in datetime
        get_datetime(datetime);

        // SENDING DATETIME
        print_datetime(datetime);
        SEND(conn_sock_fd, datetime);

        // CLOSING TRANSMISSION
        close(conn_sock_fd);
        printf("[LOG] CONNECTION CLOSED\n\n");
    }
}