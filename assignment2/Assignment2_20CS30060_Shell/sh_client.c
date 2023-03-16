#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6969
#define UNAME_SIZE 27 // 25 + 1 (\n) + 1 (\0 added by fgets in the worst case)
#define BUFFER_SIZE 50 // ALL BUFEFRS ARE NO GREATER THAN 50 IN SIZE
#define CMD_BUFFER_SIZE 200

#define SERVER_IP "127.0.0.1" // LOPBACK ADDRESS
#define PRINT_PROMPT printf
#define PRINT_RESULT printf
#define PRINT_ERR printf

#define SEND_LOG_FILE "client_send_log.txt"
#define RECV_LOG_FILE "client_recv_log.txt"
#define UIO_LOG_FILE "client_uio_log.txt"

#define SERVER_DISCONNECTED "SERVER DISCONNECTD\n"
#define VALID_NAME_REQUEST "PLEASE ENTER A NAME\n"
#define LOGIN_PROMPT "LOGIN:"
#define INVALID_COMMAND_RESPONSE "$$$$"
#define ERR_RUNNING_COMMAND_RESPONSE "####"
#define INVALID_USERNAME "Invalid username"
#define INVALID_COMMAND "Invalid command"
#define ERR_RUNNING_COMMAND "Error running command"

/*
// LOGGING MACROS 
#define BYTES_SENT(NUM_BYTES) "BYTES SENT: %d\n", NUM_BYTES
#define SEND_LOG(SEND_LOG_ENTRY,SERV_ADDR)  do { \
                                        FILE *__send_log_fptr = fopen(SEND_LOG_FILE, "a"); \
                                        time_t t = time(NULL); struct tm tm = *localtime(&t); \
                                        fprintf(__send_log_fptr, "[SEND LOG %d-%02d-%02d %02d:%02d:%02d] [SERVER %s:%d] ", \
                                            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, \
                                            inet_ntoa(SERV_ADDR.sin_addr), ntohs(SERV_ADDR.sin_port)); \
                                        fprintf(__send_log_fptr, SEND_LOG_ENTRY); \
                                        fprintf(__send_log_fptr, "\n"); \
                                        fclose(__send_log_fptr); \
                                    } while (0)

#define BYTES_RECVD(NUM_BYTES) "BYTES RECVD: %d\n", NUM_BYTES
#define TEXT_LOG(LOG_ENTRY) "%s", LOG_ENTRY
#define RECV_LOG(RECV_LOG_ENTRY,SERV_ADDR)  do { \
                                        FILE *__recv_log_fptr = fopen(RECV_LOG_FILE, "a"); \
                                        time_t t = time(NULL); struct tm tm = *localtime(&t); \
                                        fprintf(__recv_log_fptr, "[RECV LOG %d-%02d-%02d %02d:%02d:%02d] [SERVER %s:%d] ", \
                                            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, \
                                            inet_ntoa(SERV_ADDR.sin_addr), ntohs(SERV_ADDR.sin_port)); \
                                        fprintf(__recv_log_fptr, RECV_LOG_ENTRY); \
                                        fprintf(__recv_log_fptr, "\n"); \
                                        fclose(__recv_log_fptr); \
                                        } while (0)

#define UIO_LOG(UIO_LOG_ENTRY) do { \
                                    FILE *__uio_log_fptr = fopen(UIO_LOG_FILE); \
                                    printf(__uio_log_fptr, "USER IO LOG] "); \
                                    fprintf(__uio_log_fptr, UIO_LOG_ENTRY); \
                                    fprintf(__uio_log_fptr, "\n"); \
                                    fclose(__uio_log_fptr); \
                                    } while(0)
*/

// ERROR HANDLING
#define USR_INPUT_ERR do {perror("[ERROR UNIPUT] USER INPUT FAILED"); exit(EXIT_FAILURE); } while (0)
#define UNAME_ERR(CONN_SOCK_FD) do { \
                        PRINT_ERR(INVALID_USERNAME); \
                        close(CONN_SOCK_FD); \
                        exit(EXIT_FAILURE); \
                    } while (0)


// SOCKET CREATION UTILITY MACROS
#define SOCKET_ERR_PRMPT "[ERROR SOCKET CREATE] UNABLE TO CREATE SOCKET"
#define SOCKET(SOCK_FD) do { \
                                SOCK_FD = socket(PF_INET, SOCK_STREAM, 0); \
                                if (SOCK_FD < 0) { \
                                    perror(SOCKET_ERR_PRMPT); \
                                    exit(EXIT_FAILURE); \
                                } \
                            } while(0)

// SERVER ADDRESS INITIALIZATION MACROS
#define SERVER_ADDR_INIT(SERV_ADDR, IP_INIT) do { \
                                        inet_aton(IP_INIT, &SERV_ADDR.sin_addr); \
                                        SERV_ADDR.sin_port = htons(PORT); \
                                        SERV_ADDR.sin_family = AF_INET; \
                                    } while (0)

// SEND MACROS
#define SEND_ERR_PRMPT "[ERROR SEND] SEND FAILED"
#define PERSISTENT_SEND(CONN_SOCK_FD,MSG) do { \
                                                int __send_ret = 0, __bytes_sent = 0; \
                                                int __msg_size = ( strlen(MSG) + 1 ); \
                                                memset(MSG + __msg_size - 1, 0, BUFFER_SIZE - __msg_size + 1); \
                                                while (  ( __send_ret = send(CONN_SOCK_FD, MSG + __bytes_sent, __msg_size - __bytes_sent, 0) )  >  0  ) { \
                                                    __bytes_sent += __send_ret; \
                                                    if (__bytes_sent == __msg_size) break; \
                                                } \
                                                if (__send_ret < 0) perror(SEND_ERR_PRMPT); \
                                            } while (0)

// RECV MACROS
#define RECV_ERR_PRMPT "[ERROR RECV] RECV FAILED"
#define PERSISTENT_RECV(CONN_SOCK_FD,BUF,POLL) do { \
                                                POLL = 0; \
                                                int __recv_ret = 0, __bytes_recvd = 0; \
                                                memset(BUF, 0, BUFFER_SIZE); \
                                                while (  ( __recv_ret = recv(CONN_SOCK_FD, BUF + __bytes_recvd, BUFFER_SIZE - __bytes_recvd, 0) )  >  0  ) { \
                                                    __bytes_recvd += __recv_ret; \
                                                    if ( ( BUF[__bytes_recvd - 1] == '\0' ) ) { \
                                                        if ( __bytes_recvd == BUFFER_SIZE ) POLL = 1; \
                                                        break; \
                                                    } \
                                                } \
                                                if ( __recv_ret == 0 ) { \
                                                    printf(SERVER_DISCONNECTED); \
                                                    exit(EXIT_FAILURE); \
                                                } \
                                                if ( __recv_ret < 0 ) perror(RECV_ERR_PRMPT); \
                                            } while (0)

void handle_response (char *recvd_buffer) {
    // CEHCKING FOR "Invalid command" and "Error in running command"
    if ( strcmp(recvd_buffer, INVALID_COMMAND_RESPONSE) == 0 ) PRINT_ERR(INVALID_COMMAND);
    else if ( strcmp(recvd_buffer, ERR_RUNNING_COMMAND_RESPONSE) == 0 ) PRINT_ERR(ERR_RUNNING_COMMAND);
    else printf("%s", recvd_buffer);
}

#define EXHAUSTIVE_RECV(CONN_SOCK_FD,BUF,POLL) do { \
                                                                    int __cnt = 0; \
                                                                    POLL = 1; \
                                                                    memset(BUF, 0, BUFFER_SIZE); \
                                                                    while ( POLL != 0 ) { \
                                                                        __cnt ++; \
                                                                        PERSISTENT_RECV(CONN_SOCK_FD, BUF, POLL); \
                                                                        if ( __cnt == 1 ) handle_response(BUF); \
                                                                        else PRINT_RESULT("%s", BUF); \
                                                                    } \
                                                                    printf("\n"); \
                                                                } while (0)

#define EXHAUSTIVE_SEND(CONN_SOCK_FD,SEND_BUF,NET_BUF) do { \
                                                                                    int __net_str_size = (int) strlen(NET_BUF); \
                                                                                    int __idx = 0; \
                                                                                    while ( __net_str_size > 0 ) { \
                                                                                        memset(SEND_BUF, 0, BUFFER_SIZE); \
                                                                                        int i = 0; \
                                                                                        for (i = 0; ( i < (BUFFER_SIZE - 1) ) && ( i < __net_str_size ); i ++, __idx ++) { \
                                                                                            SEND_BUF[i] = NET_BUF[__idx]; \
                                                                                        } \
                                                                                        SEND_BUF[i] = '\0'; \
                                                                                        PERSISTENT_SEND(CONN_SOCK_FD, SEND_BUF); \
                                                                                        if ( __net_str_size == ( BUFFER_SIZE - 1 ) ) { \
                                                                                            char __nul_buf[1] = {0}; \
                                                                                            PERSISTENT_SEND(CONN_SOCK_FD, __nul_buf); \
                                                                                        } \
                                                                                        __net_str_size -= i; \
                                                                                    } \
                                                                                } while (0)

// CONNECT MACRO
#define SERVER_CONN_ERR do {perror("[ERROR CONNECT] UNABLE TO CONNECT TO SERVER"); exit(EXIT_FAILURE);} while (0)
#define CONNECT(SOCK_FD,SERV_ADDR) do { \
                                        int conn = connect(SOCK_FD, (struct sockaddr *) &SERV_ADDR, sizeof(SERV_ADDR)); \
                                        if (conn < 0) SERVER_CONN_ERR; \
                                    } while (0)

// USER INPUT MACRO
#define USER_INPUT_ERR do {perror("[ERROR USER INPUT] USER INPUT FAILED"); exit(EXIT_FAILURE); } while (0)
#define USER_INPUT(CMD,BUF,BUF_SIZE) do { \
                                        int __iter = 1; \
                                        while (__iter) { \
                                            __iter = 0; \
                                            char *__fgets_check = fgets(BUF, BUF_SIZE, stdin); \
                                            if ( __fgets_check == NULL ) USER_INPUT_ERR; \
                                            else { \
                                                int __buf_len = strlen(BUF); \
                                                if ( __buf_len == 1 ) { \
                                                    PRINT_PROMPT("$ "); \
                                                    __iter = 1; continue; \
                                                } \
                                                if ( BUF[__buf_len - 1] == '\n' ) BUF[__buf_len - 1] = '\0'; \
                                            } \
                                        } \
                                        strcpy(CMD, ""); \
                                        char *token = strtok(BUF, " "); \
                                        int __uio_num_toks = 0; \
                                        do { \
                                            __uio_num_toks ++; \
                                            if ( __uio_num_toks == 2 ) strcat(CMD, " "); \
                                            strcat(CMD, token); \
                                            if ( __uio_num_toks > 2 ) { \
                                                PRINT_ERR(INVALID_COMMAND); \
                                                exit(EXIT_FAILURE); \
                                            } \
                                        } while (  ( token = strtok(NULL, " ") )  ); \
                                    } while (0)

int main () {
    // DESCRIPTORS
    int sock_fd, poll;

    // ADDRESS STRUCTURE
    struct sockaddr_in serv_addr;

    // SOCKET SETUP
    SOCKET(sock_fd);

    // INITALIZING THE SERVER ADDRESS STRUCT
    SERVER_ADDR_INIT(serv_addr, SERVER_IP);

    // CONNECTION
    CONNECT(sock_fd, serv_addr);

    // THE BUFFER TO RECV IN
    char recv_buffer[BUFFER_SIZE], username[UNAME_SIZE], send_buffer[BUFFER_SIZE], uio[CMD_BUFFER_SIZE], cmd[CMD_BUFFER_SIZE];

    // STARTING THE LOGIN PROCESS
    // RECEIVING THE LOGIN PROMPT

    PERSISTENT_RECV(sock_fd, recv_buffer, poll);
    PRINT_PROMPT("%s", recv_buffer); // WILL PRINT "LOGIN" IF ALL WENT WELL

    // SENDING THE USERNAME
    // GETTIG THE USER INPUT
    int uname_len = 1;
    while ( uname_len == 1 ) { // EMPTY UNAME CHECK
        if ( fgets(username, UNAME_SIZE, stdin) == NULL ) USER_INPUT_ERR;
        uname_len = strlen(username);
        if ( username[uname_len - 1] == '\n' ) username[uname_len - 1] = '\0';
    }
    memset(uio, 0, CMD_BUFFER_SIZE);

    // SENDING THE USERNAME
    PERSISTENT_SEND(sock_fd, username);

    // RECV'ING IF THE USERNAME IS FOUND OR NOT
    PERSISTENT_RECV(sock_fd, recv_buffer, poll);
    if ( ( strcmp(recv_buffer, "NOT-FOUND") == 0 ) ) { UNAME_ERR(sock_fd); }
    else if ( strcmp(recv_buffer, "FOUND") != 0 ) { UNAME_ERR(sock_fd); }

    while(1) { // STARTING THE COMMUNICATION
        // PRINTING THE PROMPT
        PRINT_PROMPT("$ ");
        
        // USER INPUT OF THE COMMAND
        USER_INPUT(cmd, uio, CMD_BUFFER_SIZE);

        // HANDLING 'exit'
        if ( strcmp(cmd, "exit") == 0 ) break;

        // SENDING THE COMMAND
        EXHAUSTIVE_SEND(sock_fd, send_buffer, cmd);

        // RECV'ING AND HANDLING THE INPUT
        EXHAUSTIVE_RECV(sock_fd, recv_buffer, poll);
    }

   close(sock_fd);
   exit(EXIT_SUCCESS);
}