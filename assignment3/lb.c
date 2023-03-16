#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <limits.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_IP "127.0.0.1"
#define LB_IP "127.0.0.1"
#define BUFFER_SIZE 30

#define SEND_LOAD "Send Load"
#define SEND_TIME "Send Time"
#define CLIENT_DISCONNECTED "Client Disconnected"

#define LOAD_RECVD "Load received from %s(%d) %d\n"
#define SENDING_CLIENT_REQUEST "Sending client request to %s(%d)\n"

#define NUM_SERVERS 2
#define MAX_CLIENTS 2

/* utility constants*/
#define __HANDLE_ERR_IN_UTILS 1 /* if set to true, all the 
                                error messages printed in-function */

// Convert seconds to milliseconds
#define SEC_TO_MS(sec) ((sec)*1000)

// Convert nanoseconds to milliseconds
#define NS_TO_MS(ns)    ((ns)/1000000)

/* Error messages*/
#define SOCKET_ERR_MSG "[ERROR] socket creation failed"
#define BIND_ERR_MSG "[ERROR] bind failed"
#define CONNECT_ERR_MSG "[ERROR] connect failed"
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

#define CONNECT_ERR                 \
    do {                            \
        perror(CONNECT_ERR_MSG);    \
        exit(EXIT_FAILURE);         \
    } while (0)

#define ACCEPT_ERR                  \
    do {                            \
        perror(ACCEPT_ERR_MSG);     \
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
    while ( ( __recv_ret = recv(sock_fd                     /* descriptor*/
                                , recv_buffer + bytes_recvd /*address to write to changes*/ 
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

/**
 * utility that initializes the server address in a structure, and binds it to a socket
 * 
 * @param sockaddr_in [out] the address struct (sockaddr_in)
 * @param server_family [in] the socket family (int) of the address
 * @param server_ip [in] the IP in string (char *) form
 * @param server_port [in] the port (int)
 * @return result of the bind operation
*/

void server_init(struct sockaddr_in *server_addr
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
}

/**
 * filling up the table by asking servers for sending their loads
 * 
 * clients connect to the servers and then sends request for 
 * sending the loads, then recv's the loads, closes the connection
 * 
 * @param server_loads [out] the array (int *) to store the loads
 * @param server_addrs [in] the server addresses (sockaddr_in)
 * @param num_servers [in] the number (int) of servers
 * @return none
 * 
*/

void update_loads(int *server_loads, struct sockaddr_in *server_addrs, int num_servers) {
    
    // iterating over the servers to get the load
    for ( int server_idx = 0; server_idx < num_servers; server_idx ++ ) {

        int server_sock_fd = socket(
                                    PF_INET
                                    , SOCK_STREAM
                                    , 0
                                    );

        if ( server_sock_fd < 0 )
            SOCKET_ERR;                                     // Error handling

        // connect'ing to server number server_idx
        if ( connect( 
                    server_sock_fd                          /* descriptor*/
                    , (struct sockaddr *) &server_addrs[server_idx]
                    , sizeof (server_addrs[server_idx])
                    ) < 0 
                    )
            CONNECT_ERR;                                    // Error handling

        // sending request to the server
        int __send_ret;

        persistent_send(
                        server_sock_fd
                        , SEND_LOAD
                        , strlen(SEND_LOAD) + 1
                        , &__send_ret
                        );

        // variables to handle recv of load
        int __recv_ret;
        unsigned int recv_buffer;
        size_t recv_buffer_size = sizeof(recv_buffer);

        persistent_recv(
                        server_sock_fd
                        , (char *) &recv_buffer
                        , recv_buffer_size
                        , '\0'
                        , &__recv_ret
                        );

        // recv'd load
        server_loads[server_idx] = ntohs(recv_buffer);
        printf(
                LOAD_RECVD
                , inet_ntoa(server_addrs[server_idx].sin_addr)
                , server_idx
                , server_loads[server_idx]
                );
        
        close(server_sock_fd);
    }
}

/**
 * gets the server with currently the minimum load, per the 
 * "server_loads" table.
 * 
 * @param server_loads [in] the array (int *) of servers' loads
 * @param num_servers [in] the number (int) of servers
 * @return min_server, the server with the minimum load
*/

int get_server_with_min_load(int *server_loads, int num_servers) {
    
    // required variables
    int min_load = INT_MAX, min_server = -1;                /*!< the util variables*/

    for (int server_idx = 0; server_idx < num_servers; server_idx ++) {
        
        // if we need to change the value
        if ( server_loads[server_idx] < min_load ) {
            
            // new minimum load server
            min_load = server_loads[server_idx];
            min_server = server_idx;
        }
    }

    return min_server;
}

/**
 * utility function to give the system time in milliseconds, used
 * for 
*/

uint32_t millis()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint32_t ms = SEC_TO_MS((uint64_t)ts.tv_sec) + NS_TO_MS((uint64_t)ts.tv_nsec);
    return ms;
}

int main (int argc, char **argv) {
    int lb_fd, client_fd;                                   /*!< socket descriptors*/

    struct sockaddr_in lb_addr, server_addrs[NUM_SERVERS], client_addr;
                                                            /*!< address structs*/

    int server_load[NUM_SERVERS];                           /*!< the table containing 
                                                            last known server load*/
    int server_ports[NUM_SERVERS];                          /*!< the ports of the servers*/

    // error check
    if ( argc != NUM_SERVERS + 2) {
        printf("[ERROR] Incorrect number of commandline arguments! Exiting\n");
        exit(EXIT_FAILURE);
    }

    // taking port as input from CLI
    int lb_port = atoi(argv[1]);

    for (int i = 2; i < argc; i ++)
        server_ports[i - 2] = atoi(argv[i]);

    // socket created for listening on
    if ( ( lb_fd = socket(
                        PF_INET
                        , SOCK_STREAM
                        , 0
                        ) ) < 0 )
        SOCKET_ERR;                                         // Error handling
    
    // binding the load balancer to the socket
    server_init_and_bind(
                        lb_fd
                        , &lb_addr
                        , AF_INET
                        , LB_IP
                        , lb_port
                        );

    // connecting to the servers
    for ( int server_idx = 0; server_idx < NUM_SERVERS; server_idx ++ )

        // initializing the server addresses
        server_init(
                    &server_addrs[server_idx]
                    , AF_INET
                    , SERVER_IP
                    , server_ports[server_idx]
                    );

    // getting load
    update_loads(server_load, server_addrs, NUM_SERVERS);

    // polling for the client connections
    struct pollfd lb_poll_fd[1];

    lb_poll_fd[0].fd = lb_fd;
    lb_poll_fd[0].events = POLLIN | POLLHUP;

    // time left till next server look-up
    int time_left = 5000;

    // listen'ing
    listen(lb_fd, 5);

    while ( 1 ) {

        // polling for 5 seconds
        uint32_t poll_start = millis();

        int poll_return = poll(
                                lb_poll_fd
                                , 1
                                , time_left
                                );

        if ( poll_return < 0 ) {
            perror("[ERROR] poll failed");
            exit(EXIT_FAILURE);
        }

        // printf("time left: %d\n", time_left);

        // printf("TIME_LEFT: %d\n", time_left);

        if ( poll_return  > 0 ) {

            // accept'ing connections
            socklen_t client_address_len = sizeof(client_addr);
            
            client_fd = accept(
                                lb_fd
                                , (struct sockaddr *) &client_addr
                                , &client_address_len
                                );

            // accept failed
            if ( client_fd < 0 )
                ACCEPT_ERR;

            int server_idx = get_server_with_min_load(
                                                        server_load
                                                        , NUM_SERVERS
                                                    );
            if ( fork() == 0 ) {

                // closing listening socket
                close(lb_fd);

                // connecting to sever in question
                int server_sock_fd = socket(
                                            PF_INET
                                            , SOCK_STREAM
                                            , 0
                                            );

                if ( server_sock_fd < 0 )
                    SOCKET_ERR;                                     // Error handling

                // connect'ing to server number server_idx
                if ( connect( 
                            server_sock_fd                          /* descriptor*/
                            , (struct sockaddr *) &server_addrs[server_idx]
                            , sizeof (server_addrs[server_idx])
                            ) < 0 
                            )
                    CONNECT_ERR;                                    // Error handling

                printf(
                        SENDING_CLIENT_REQUEST
                        , inet_ntoa(server_addrs[server_idx].sin_addr)
                        , server_idx
                        );
                
                // sending request to the server
                int __send_ret;
                persistent_send(
                                server_sock_fd
                                , SEND_TIME
                                , strlen(SEND_TIME) + 1
                                , &__send_ret
                                );
                
                // getting the datetime
                int __recv_ret;
                char datetime[BUFFER_SIZE];
                persistent_recv(
                                server_sock_fd
                                , datetime
                                , BUFFER_SIZE
                                , '\0'
                                , &__recv_ret
                                );

                // closing connection to server
                close(server_sock_fd);

                // sending it to the client
                persistent_send(
                                client_fd
                                , datetime
                                , strlen(datetime) + 1
                                , &__send_ret
                                );
                
                // closing the connection to client
                close(client_fd);

                exit(EXIT_SUCCESS);
            }

            uint32_t poll_end = millis();
        
            // updating the lookup time
            time_left -= (poll_end - poll_start);

            //printf("[LOG] time_left: %d\n", time_left);
        }
        else {
            update_loads(
                        server_load
                        , server_addrs
                        , NUM_SERVERS
                        );
            // refreshing time_left
            time_left = 5000;
        }

        /**
         * if the time taken during the (consecutive) client requests exceeds 5sec,
         * the poll call will start blocking, indefinitely hence, in that case, we 
         * need poll to immediately return (timeout), and get new loads.
        */
        time_left = (time_left < 0) ? 0 : time_left;
    }

    return 0;
}