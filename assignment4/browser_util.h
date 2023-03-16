#ifndef __BROWSER_UTILS
#define __BROWSER_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h> 

#define IMG 1
#define HTML 2
#define PDF 3
#define MISC 4

#define HTTP "HTTP/1.1"
#define HTTP_START "http://"
#define DEFAULT "/"

#define DATE_LEN 200
#define BUFFER_SIZE 100
#define DIR_LEN 300

#define __HANDLE_ERR_IN_UTILS 1

#define SEND_ERR_MSG "[ERROR] send failed"
#define RECV_ERR_MSG "[ERROR] recv failed"

#define APPND_SPACE                         \
    do {                                    \
            append_to_http_request(" ");    \
    } while (0)

#define APPND_NWLINE                        \
    do {                                    \
            append_to_http_request("\r\n");   \
    } while (0)

#define FILE_ERR                                \
    do {                                        \
        printf("[ERROR] FILE DOES'NT EXIST");   \
        exit(EXIT_FAILURE);                     \
    } while (0)

#define RESP_VER_ERR                            \
    do                                          \
    {                                           \
        printf("[ERROR] response version\n");   \
        exit(EXIT_FAILURE);                     \
    } while (0)

#define RESPON_ERR                       \
    do                                   \
    {                                    \
        perror("[ERROR] response NULL"); \
        exit(EXIT_FAILURE);              \
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
 * Creates the TCP Connection to the server, and sends the
 * HTTP request to it, and then waits to recieve the response
 */

void handle_connection(
    char *host, char *path, char *req, int port);

/**
 * Creates the HTTP request for the given file type (determined
 * by the path variable), and the host (including port)
 */

char *create_http_request();

/**
 * Parses the recvd response from the server, saves the file
 * and displays it
 */

void handle_response(char *response);

/**
 * file saving utility
 */

int save_file();

/**
 * file opening utility, for keeping it general
 */

void open_file(char *filename);

int send_chunks(int sock_fd, void *final, int buffer_size, int final_size);

#endif