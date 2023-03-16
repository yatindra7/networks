#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define LB_IP "127.0.0.1"
#define BUFFER_SIZE 50

/* utility constants*/
#define __HANDLE_ERR_IN_UTILS 1 /* if set to true, all the 
                                error messages printed in-function */

/* Error messages*/
#define SOCKET_ERR_MSG "[ERROR] socket creation failed"
#define CONNECT_ERR_MSG "[ERROR] connect failed"

#define SEND_ERR_MSG "[ERROR] send failed"
#define RECV_ERR_MSG "[ERROR] recv failed"
#define IP_CONV_ERR_MSG "[ERROR] inet_pton, failed"

/* Error actions*/
#define SOCKET_ERR                  \
    do {                            \
        perror(SOCKET_ERR_MSG);     \
        exit(EXIT_FAILURE);         \
    } while (0)

#define CONNECT_ERR                 \
    do {                            \
        perror(CONNECT_ERR_MSG);    \
        exit(EXIT_FAILURE);         \
    } while (0)

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
    while ( ( __recv_ret = recv(
                                sock_fd
                                , recv_buffer + bytes_recvd /*address to write to changes*/ 
                                , buffer_size - bytes_recvd /*bytes left in byffer*/
                                , 0
                                ) ) > 0 ) {
    
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
 * @param sockaddr_in [out] the address struct (sockaddr_in)
 * @param server_family [in] the socket family (int) of the address
 * @param server_ip [in] the IP in string (char *) form
 * @param server_port [in] the port (int)
 * @return result of the bind operation
*/

void server_init(struct sockaddr_in server_addr
                , char *server_ip, int server_port) {
    
    // address family
    server_addr.sin_family = AF_INET;
    
    // converting the IP to network form and giving to the struct
    if ( inet_pton(AF_INET, server_ip, &server_addr.sin_addr) < 0 ) {
        perror(IP_CONV_ERR_MSG);
        exit(EXIT_FAILURE);
    }

    // setting up the port
    server_addr.sin_port = htons(server_port);
}

/**
 * utility to print the datetime string
 * 
 * @param datetime [in] the datetime string (char *) to be printed
*/
void print_datetime(char *datetime) {
    printf("%s\n", datetime);
}

int main (int argc, char **argv) {

    int lb_port;                                    // port

    if ( argc != 2 ) {
        printf("[ERROR] Incorrect number of arguments\n");
        exit(EXIT_FAILURE);
    } else
        lb_port = atoi(argv[1]);

    int sock_fd;                                    // socket descriptor
    int recv_ret;                                   // to monitor the recv
    struct sockaddr_in server_addr;                 // struct for server address
    char datetime[BUFFER_SIZE];                     // datetime buffer

    // socket create
    if ( ( sock_fd = socket(
                            PF_INET
                            , SOCK_STREAM
                            , 0
                            ) ) < 0 )
        SOCKET_ERR;                                 // Error handling

    // socket address setup
    //server_init(server_addr, LB_IP, lb_port);

    // address family
    server_addr.sin_family = AF_INET;
    
    // converting the IP to network form and giving to the struct
    if ( inet_pton(AF_INET, LB_IP, &server_addr.sin_addr) < 0 ) {
        perror(IP_CONV_ERR_MSG);
        exit(EXIT_FAILURE);
    }

    // setting up the port
    server_addr.sin_port = htons(lb_port);

    // connect
    if ( connect(
                sock_fd
                , (struct sockaddr *) &server_addr
                , sizeof (server_addr)
                ) < 0 )
        CONNECT_ERR;

    // recv the datetime
    persistent_recv(
                    sock_fd
                    , datetime
                    , BUFFER_SIZE
                    , '\0'
                    , &recv_ret
                    );
    
    if ( recv_ret < 0 ) {
        printf("Exiting because recv failed\n");
        exit(EXIT_FAILURE);                         // checking if the recv failed
    }

    print_datetime(datetime);                       // printing recv'd datetime
    exit(EXIT_SUCCESS);
}