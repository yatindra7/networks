#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_IP "127.0.0.1"
#define SEND_LOAD "Send Load"
#define SEND_TIME "Send Time"
#define CLIENT_DISCONNECTED "Client Disconnected\n"

#define LOAD_SENT "Load sent %d\n"

/* utility constants*/
#define MAX_LOAD 100
#define BUFFER_SIZE 30
#define __HANDLE_ERR_IN_UTILS 1 /* if set to true, all the 
                                error messages printed in-function */

/* Error messages*/
#define SOCKET_ERR_MSG "[ERROR] socket creation failed"
#define BIND_ERR_MSG "[ERROR] bind failed"
#define ACCEPT_ERR_MSG "[ERROR] accept failed"

#define SEND_ERR_MSG "[ERROR] send failed"
#define RECV_ERR_MSG "[ERROR] recv failed"
#define IP_CONV_ERR_MSG "[ERROR] inet_pton, failed"

/* Error actions*/
#define SOCKET_ERR                  \
    do {                            \
        perror(SOCKET_ERR_MSG);     \
        exit(EXIT_FAILURE);         \
    } while (0)

#define ACCEPT_ERR                  \
    do {                            \
        perror(ACCEPT_ERR_MSG);     \
        exit(EXIT_FAILURE);         \
    } while (0)

/**
 * function to generate dummy load for server
*/
unsigned int get_server_load() {
    return rand() % ( MAX_LOAD + 1 ); // random value between 0 and MAX_LOAD
}

/**
 * function stores the current system time into the given string
 * 
 * @param buf [out] the buffer (char *) to store the datetime
 * @return the return value of sprintf, to check the buffer size
*/

int get_datetime(char *buf) {
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    // copying the datetime
    return sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d"
                        , tm.tm_year + 1900             // year
                        , tm.tm_mon + 1                 // month
                        , tm.tm_mday                    // day
                        , tm.tm_hour                    // hour
                        , tm.tm_min                     // min
                        , tm.tm_sec                     // second
                    );
}

/**
 * TCP send function alternative that doesn't return till the data is sent
 * 
 * "persistent_send" deals with the TCP send's shortcoming of not always 
 * sending the whole data at once. This function persistently makes "send"
 * calls till all the data is sent.
 * 
 * The last return value of the send calls is returned via a pointer param,
 * but the error message display is handled already.
 * 
 * eg: "send" can send 10 bytes as "4, 6", or "2, 3, 5" or anything else,
 * persistent send will repeatedly make send calls, while keeping track of 
 * the bytes sent.
 * 
 * \example persistent_send.c
 * 
 * @param sock_fd [in] the descriptor (int) for TCP socket
 * @param msg [in] the buffer (char []) to be sent
 * @param msg_size [in] the number of bytes (size_t) to be sent
 * @param send_ret [out] (int*), address for return of the last send call
 * @return bytes_sent (int), number of bytes finally sent
 * 
*/

int persistent_send(int sock_fd, char *msg, size_t msg_size, int *send_ret) {

    int __send_ret;                                   /*!< holds the return value
                                                         of send for error check*/
    int bytes_sent = 0;                               /*!< [out] bytes sent */

    // send'ing in a loop
    while ( ( __send_ret = send(
                                sock_fd
                                , msg
                                , msg_size - bytes_sent
                                , 0
                                ) ) > 0 )
        bytes_sent += __send_ret;                       /* incrementing bytes_recvd*/

    // Returning
    if ( __send_ret < 0 && __HANDLE_ERR_IN_UTILS )
        perror(SEND_ERR_MSG);                           /* Error handling */
    *send_ret = __send_ret;                             /* setting up send_ret*/
    return bytes_sent;
}

/**
 * TCP recv function alternative that only returns if either the last recv'd byte
 * is the "end" byte, or if the buffer is full
 * 
 * "presistent_recv" deals with TCP recv returning instantaneously returning with 
 * data available, hence possibly recv'ing the data in chunks.
 * 
 * eg: sender sends 10 bytes, recv might need to called twice and can return with
 * "4, 6", or "2, 3, 5" or any other combination.
 * 
 * "persistent_recv" akes repeated recv calls with a given buffer size, until
 * either the buffer fills up, or a delimiter is reached.
 * 
 * \example persistent_recv.c
 * 
 * @param sock_fd [in] the descriptor (int) for TCP soxket
 * @param recv_buffer [out] address of the buffer (char*) to read into
 * @param buffer_size [in] the number of bytes that can be recv'd
 * @param end [in] the character (char) signifying the stream end 
 * @param recv_ret [out] (int*), address for return of the last recv call
 * @return bytes_recvd (int) number of bytes finally recv'd
 * 
*/

int persistent_recv(int sock_fd, char *recv_buffer, size_t buffer_size
                    , char end, int *recv_ret) {

    int bytes_recvd = 0;                            /*!< [out] number of bytes recvd*/
    int __recv_ret;                                 /*!< keeps track of recv return 
                                                        values for error handling*/

    // recv'ing in loop
    while ( ( __recv_ret = recv(sock_fd, recv_buffer + bytes_recvd /*address to write to changes*/ 
                                , buffer_size - bytes_recvd /*bytes left in byffer*/, 0) ) > 0 ) {
    
        bytes_recvd += __recv_ret;                  /* incrementing bytes_recvd*/
        
        // Checking the break condition
        if ( recv_buffer[bytes_recvd - 1] == end ) break;
        if ( bytes_recvd == buffer_size ) break;
    }

    // Returning
    if ( __recv_ret < 0 && __HANDLE_ERR_IN_UTILS )
        perror(RECV_ERR_MSG);                       /* Error handling*/
    *recv_ret = __recv_ret;                         /* setting up recv_ret*/
    return bytes_recvd;
}

/**
 * utility that initializes the server address in a structure, and binds it to a socket
 * 
 * @param sock_fd [in] the descriptor (int) to bind to 
 * @param sockaddr_in [out] the address struct (sockaddr_in)
 * @param server_family [in] the socket family (int) of the address
 * @param server_ip [in] the IP in string (char *) form
 * @param server_port [in] the port (int)
 * @return result of the bind operation
*/

int server_init_and_bind(int sock_fd, struct sockaddr_in *server_addr
                    , int server_family, char *server_ip, int server_port) {
    
    // converting the IP to network form and giving to the struct
    if ( inet_pton(AF_INET, server_ip, &(server_addr -> sin_addr) ) < 0 ) {
        perror(IP_CONV_ERR_MSG);
        exit(EXIT_FAILURE);
    }

    // setting up the port
    server_addr -> sin_port = htons(server_port);

    // address family
    server_addr -> sin_family = server_family;

    // running bind
    int bind_result = bind(
                            sock_fd
                            , (struct sockaddr *) server_addr
                            , sizeof (*server_addr)
                            );

    if ( bind_result < 0 && __HANDLE_ERR_IN_UTILS ) { // Error handling for bind
        perror(BIND_ERR_MSG);
        exit(EXIT_FAILURE);
    }

    return bind_result;
}

int main (int argc, char **argv) {

    int port;                                       /*!< the port */

    // checking the number of args given
    if ( argc != 2 ) {
        printf("Incorrect number of arguments, exiting\n");
        exit(EXIT_FAILURE);
    } else                                          // if the port is being passed correctly
        port = atoi(argv[1]);

    int sock_fd, conn_sock_fd;                      /*!< the descriptors */
    struct sockaddr_in server_addr, client_addr;    /*!< structs for client
                                                        and server address*/

    // recv buffer, and datetime buffer
    char recv_buffer[BUFFER_SIZE], datetime[BUFFER_SIZE];

    // socket creation
    sock_fd = socket(
                    PF_INET
                    , SOCK_STREAM
                    , 0
                    );

    if ( sock_fd < 0 )                              // Error check
        SOCKET_ERR;

    // initializing the server and binding
    server_init_and_bind(
                         sock_fd
                        , &server_addr
                        , AF_INET
                        , SERVER_IP
                        , port
                        );

    // listening
    listen(sock_fd, 5);

    while ( 1 ) {

        // size to be passed in connect
        socklen_t client_addr_len = sizeof (client_addr);

        // accepting connections
        conn_sock_fd = accept(
                            sock_fd
                            , (struct sockaddr *) &client_addr
                            , &client_addr_len
                            );

        if ( conn_sock_fd < 0 )                     // Error check
            ACCEPT_ERR;

        /**
         * 1. we will now first recv the request from the client
         * 2. client in this case is the load balancer
         * 3. on the basis of the request recv'd, we'll send 
         * appropriate information (Time or Load)
        */

       int recv_ret, send_ret;                      /*!< the vars 
                                                        for return value */

        // looping in case of multiple failed attempts 
        while ( 1 ) {
            persistent_recv(
                            conn_sock_fd
                            , recv_buffer
                            , BUFFER_SIZE
                            , '\0'
                            , &recv_ret
                            );

            // checking for disconnect
            if ( recv_ret == 0 ) {
                printf(CLIENT_DISCONNECTED);
                break;
            }

            if ( strcmp(recv_buffer, SEND_LOAD) == 0 ) {

                // getting the system load
                unsigned int load = htons(
                                        get_server_load()
                                        );

                // size of the load variable
                size_t load_buf_size = sizeof (load);

                // sending the same
                persistent_send(
                                conn_sock_fd
                                , (char *) &load
                                , load_buf_size
                                , &send_ret
                                );
                
                // printing to CLI
                printf(LOAD_SENT, ntohs(load) );

            } else if ( strcmp(recv_buffer, SEND_TIME) == 0 ) {

                // getting the datetime
                int datetime_size = get_datetime(datetime);

                persistent_send(
                                conn_sock_fd
                                , datetime
                                , datetime_size + 1
                                , &send_ret
                                );
                
                printf("Time sent, %s\n", datetime);

            } else {
                /* Do nothing, incorrect prompt*/
            }
        }

        // close the connection
        close(conn_sock_fd);
    }
}