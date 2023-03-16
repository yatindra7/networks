#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//CONSTANTS
#define BUF_SIZE 20
#define UIO_SIZE (BUF_SIZE - 1)
#define SERVER_PORT htons(2000)
#define SERVER_IP "127.0.0.1"
#define RECV_BUF_SIZE 100
#define SEND_BUF_SIZE 100

// ERROR HANDLING MACROS
#define SOCK_CREATE_ERR do {perror("[ERROR] UNABLE TO OPEN SOCKET "); exit(EXIT_FAILURE);} while (0)
#define SERVER_CONN_ERR do {perror("[ERROR] UNABLE TO CONNECT TO SERVER "); exit(EXIT_FAILURE);} while (0)

#define MALLOC_ERR do { \
                        perror("[ERROR] MALLOC FAILED "); \
                        exit(EXIT_FAILURE); \
                        } while(0)
#define REALLOC_ERR do { \
                        perror("[ERROR] REALLOC FAILED "); \
                        exit(EXIT_FAILURE); \
                        } while(0)

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
#define SEND(SOCK_FD,MSG) do { \
                                int __bytes_sent = 0; \
                                while (1) { \
                                    __bytes_sent += send(SOCK_FD, (MSG) + __bytes_sent, (strlen(MSG) + 1) - __bytes_sent, 0); \
                                    if (__bytes_sent == (strlen(MSG) + 1) ) break; \
                                } \
                            } while (0)

                                
#define RECV(SOCK_FD,BUF) recv(SOCK_FD, BUF, RECV_BUF_SIZE, 0)

// malloc MACRO
#define MALLOC_SAFE(TYPE,NUM,PTR) do { \
                                        TYPE *_tmp = (TYPE *) malloc ((NUM) * sizeof (TYPE)); \
                                        if (_tmp == NULL) MALLOC_ERR; \
                                        else PTR = _tmp; \
                                        } while(0)

// realloc MACRO
#define REALLOC_SAFE(TYPE,NUM,INIT_PTR) do { \
                                        void *_tmp = realloc(INIT_PTR,(NUM) * sizeof(TYPE)); \
                                        if (_tmp == NULL) REALLOC_ERR; \
                                        else INIT_PTR = _tmp; \
                                        } while (0)

char *variable_length_input() {

    printf("[LOG] variable_length_input commence\nINPUT$ ");
    char tmp_buf[BUF_SIZE];
    int current_buf_size = 0;
    
    // malloc'ing the buffer
    char *buf;
    char *res;
    MALLOC_SAFE(char, BUF_SIZE, buf);

    current_buf_size += BUF_SIZE;
    memset(buf, 0, BUF_SIZE); // Empty string to initialize the buffer


    while ((res = fgets(tmp_buf, BUF_SIZE, stdin)) != NULL) {
        // CHECK IF BUFFER IS NOT COMPLETELY FULL

        /*
            Note this also includes the '\n'
            Example: strlen("CAFE\n") == 5; (and not 4)
            Hence the '\n' is at the index "strlen - 1"
        */
        int offset = strlen(tmp_buf);
        //printf("[LOG] offset is %d\n", offset);

        // Adding the strings
        strcat(buf, tmp_buf);
        //printf("[LOG] Concatenation done, buf is %lu bytes\n", strlen(buf));

        // The read is done
        if (offset != UIO_SIZE) {
            int end = (((int)strlen(buf)) - 1);
            buf[end] = '\0';
            printf("[LOG] Concatenation done, buf is %d bytes\n", end);
            break;
        }

        // realloc for increasing the "buf" size
        current_buf_size += BUF_SIZE;
        REALLOC_SAFE(char, current_buf_size, buf);
        //printf("[LOG] realloc'd@%p, and current_buf_size is %d\n", buf, current_buf_size);
    }

    return buf;
}

int main () {
    // THE socket_descriptor
    int sock_fd;

    // TAKING THE INPUT EXPRESSION
    char *expr = variable_length_input();

    // ADDRESS
    struct sockaddr_in serv_addr;

    // CREATING SOCKET
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) SOCK_CREATE_ERR;

    // CREATING THE SERVER ADDR STRUCT
    INIT_SERV_ADDR(serv_addr);

    // CONNECTING TO THE SERVER
    CONNECT(sock_fd, serv_addr);

    // SENDING THE EXPRESSION TO THE SERVER
    SEND(sock_fd, expr);

    printf("[LOG] ALL DATA SENT, RECEIVING RESULT NOW\n");

    // ALLOCATING BUFFER TO COLLECT THE ANSWER

    char *answer;
    size_t sz = sizeof(double);
    MALLOC_SAFE(char, sz, answer);
    memset(answer, 0, sz);

    // recv'ing THE VALUE BACK
    
    //recv(sock_fd, answer, sz, 0);
    printf("[LOG] ALL DATA SENT, RECEIVD RESULTS\n");


    int bytes_recvd = 0;
    while ((bytes_recvd = recv(sock_fd, answer + bytes_recvd, sz - bytes_recvd, 0))) {

        if (bytes_recvd < 0) {
            perror("[ERROR] recv error");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        if (bytes_recvd == 0) {
            perror("[ERROR] recv error, conn closed");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        printf("[RECV_LOG] bytes_recvd: %d\n", bytes_recvd);
        //printf("[ANS] %s\n", answer);
        //if (bytes_recvd == sz) break;
    }

    printf("[ANSWER] %s\n", answer);
}