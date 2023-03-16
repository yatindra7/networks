#ifndef __BROWSER_UTILS
#define __BROWSER_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define IMG 1
#define HTML 2
#define PDF 3
#define MISC 4
#define HTTP "HTTP/1.1"
#define HTTP_START "http://"
#define DATE_LEN 200

#define APPND_SPACE                         \
    do {                                    \
            append_to_http_request(" ");    \
    } while (0)

#define APPND_NWLINE                        \
    do {                                    \
            append_to_http_request("\n");   \
    } while (0)

#define FILE_ERR                                \
    do {                                        \
        printf("[ERROR] FILE DOES'NT EXIST");   \
        exit(EXIT_FAILURE);                     \
    } while (0)

#define STRCAT_ERR                       \
    do                                   \
    {                                    \
        perror("[ERROR] strcat failed"); \
        exit(EXIT_FAILURE);              \
    } while (0)

#define MALLOC_ERR                        \
    do                                    \
    {                                     \
        perror("[ERROR] MALLOC FAILED "); \
        exit(EXIT_FAILURE);               \
    } while (0)

#define REALLOC_ERR                        \
    do                                     \
    {                                      \
        perror("[ERROR] REALLOC FAILED "); \
        exit(EXIT_FAILURE);                \
    } while (0)

// malloc MACRO
#define MALLOC_SAFE(TYPE, NUM, PTR)                        \
    do                                                     \
    {                                                      \
        TYPE *_tmp = (TYPE *)malloc((NUM) * sizeof(TYPE)); \
        if (_tmp == NULL)                                  \
            MALLOC_ERR;                                    \
        else                                               \
            PTR = _tmp;                                    \
    } while (0)

// realloc MACRO
#define REALLOC_SAFE(TYPE, NUM, INIT_PTR)                     \
    do                                                        \
    {                                                         \
        void *_tmp = realloc(INIT_PTR, (NUM) * sizeof(TYPE)); \
        if (_tmp == NULL)                                     \
            REALLOC_ERR;                                      \
        else                                                  \
            INIT_PTR = _tmp;                                  \
    } while (0)

// SOCKET CREATION UTILITY MACROS
#define SOCKET_ERR_PRMPT "[ERROR SOCKET CREATE] UNABLE TO CREATE SOCKET"
#define SOCKET(SOCK_FD)                            \
    do                                             \
    {                                              \
        SOCK_FD = socket(PF_INET, SOCK_STREAM, 0); \
        if (SOCK_FD < 0)                           \
        {                                          \
            perror(SOCKET_ERR_PRMPT);              \
            exit(EXIT_FAILURE);                    \
        }                                          \
    } while (0)

// SERVER ADDRESS INITIALIZATION MACROS
#define SERVER_ADDR_INIT(SERV_ADDR, IP_INIT, PORT) \
    do                                             \
    {                                              \
        SERV_ADDR.sin_addr = IP_INIT;              \
        SERV_ADDR.sin_port = htons(PORT);          \
        SERV_ADDR.sin_family = AF_INET;            \
    } while (0)

// CONNECT MACRO
#define SERVER_CONN_ERR                                        \
    do                                                         \
    {                                                          \
        perror("[ERROR CONNECT] UNABLE TO CONNECT TO SERVER"); \
        exit(EXIT_FAILURE);                                    \
    } while (0)
#define CONNECT(SOCK_FD, SERV_ADDR)                                                    \
    do                                                                                 \
    {                                                                                  \
        int conn = connect(SOCK_FD, (struct sockaddr *)&SERV_ADDR, sizeof(SERV_ADDR)); \
        if (conn < 0)                                                                  \
            SERVER_CONN_ERR;                                                           \
    } while (0)

// FUNCTIONS

/**
 * Maintains the TCP Connection to the client, waits to 
 * recieve the request
 */

void handle_connection(
    char *host, char *path, char *req, int port);

#endif