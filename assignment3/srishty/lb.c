#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NUM_SERV 2

int get_server_loads(int *server_loads, struct sockaddr_in *servaddr) {

    char send_load[20] = "Send Load";
    int load_recvd;

    for (int i = 0; i < NUM_SERV; i ++) {

        // Error check

        int servfd = socket(PF_INET, SOCK_STREAM, 0);
        if (servfd < 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (connect(servfd, (struct sockaddr *) &servaddr[i], sizeof (servaddr[i])) < 0) {
            perror("connect failed");
            exit(EXIT_FAILURE);
        }

        // printf("[get loads] server i: %d\n", i);
        // requesting the servers for loads
        int num_tries = 0, ret, len_send = 0;
        size_t to_be_sent = strlen(send_load) + 1;

        // printf("Sending request to %d\n", i);
        // sending the request to the server
        // retry
        while (num_tries < 5) {
            // printf("[LOAD DEBUG]Send loop try: numtry: %d\n", num_tries + 1);
            // printf("[LOAD DEBUG]send_load: %s, to_be_sent: %d\n", send_load, to_be_sent);
            
            // looping tell all bytes sent
            while ( (ret = send(servfd // file descriptor
                            , send_load + len_send // the buffer to be sent
                            , to_be_sent - len_send // number of bytes to be sent 
                            , 0 // flag
                            )) > 0 ) {
                len_send += ret;
                // printf("[LOAD DEBUG][send] len_sent %d\n", len_send);
                if (len_send == to_be_sent) break;
            }
            // printf("[LOAD DEBUG] first send loop done, ret is %d\n", ret);
            if (ret > 0) break;
            num_tries ++;
        }

        // printf("[LOAD DEBUG]Error check to be done\n");
        if (ret < 0) {
            perror("send failed");
            return -1;
        }

        // printf("[LOAD DEBUG]send done\n");
        int len_recvd = 0;
        size_t to_be_recvd = sizeof(int);
        num_tries = 0;
        
        // printf("[LOAD DEBUG]recving load\n");
        // recv'ing 
        while (num_tries < 5) {
            
            // looping to recv all the bytes
            while ( (ret = recv(servfd
                                , &load_recvd + len_recvd
                                , to_be_recvd - len_recvd
                                , 0
                                )) > 0 ) {
                len_recvd += ret;
                if (len_recvd == to_be_recvd) break;
            }
            if (ret > 0) break;
            num_tries ++;
        }

        if (ret < 0) {
            perror("recv failed");
            return -1;
        }

        close(servfd);

        server_loads[i] = ntohs(load_recvd);
        printf("Load received from %s[%d] %d\n", inet_ntoa(servaddr[i].sin_addr), i, server_loads[i]);
    }
    return 0;
}

int server_with_min_load(int *server_loads) {
    int min_load = INT_MAX, min_index = -1;

    // finding the server with minimum load
    for (int i = 0; i < NUM_SERV; i ++) {
        if (server_loads[i] < min_load) {
            min_index = i; min_load = server_loads[i];
        }
    }

    return min_index;
}

int main (int argc, char **argv) {

    if (argc != 4) {
        printf("Error, incorrect number of commandline arguments\n");
        exit(EXIT_FAILURE);
    }

    int lbfd, clifd;
    int ret, counter_time, time_elapsed;
    struct sockaddr_in lbaddr, servaddr[NUM_SERV], cliaddr;
    int server_loads[NUM_SERV];

    // load balancer socket
    // this is the socket on which the load balancer 
    // listens for new connections
    lbfd = socket(PF_INET, SOCK_STREAM, 0);
    if (lbfd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // lb and server address init
    lbaddr.sin_family = AF_INET;
    lbaddr.sin_port = htons(atoi(argv[1]));
    inet_pton(AF_INET, "127.0.0.1", &lbaddr.sin_addr);

    for (int i = 0; i < NUM_SERV; i ++) {

        servaddr[i].sin_family = AF_INET;
        servaddr[i].sin_port = htons(atoi(argv[i + 2]));
        inet_pton(AF_INET, "127.0.0.1", &servaddr[i].sin_addr);
    }

    /*
    // connect'ing to the servers
    for (int i = 0; i < NUM_SERV; i ++) {
        
        // Error check
        if (connect(servfd[i], (struct sockaddr *) &servaddr[i], sizeof (servaddr[i])) < 0) {
            perror("connect failed");
            exit(EXIT_FAILURE);
        }
    }
    */

    // now we listen for the connections on lbfd
    // we start by binding on lbfd

    if (bind(lbfd, (struct sockaddr *) &lbaddr, sizeof(lbaddr)) < 0) {
        perror("lbfd bind failed");
        exit(EXIT_FAILURE);
    }

    // setting up poll on accepting
    struct pollfd acc_fd[1];
    acc_fd[0].fd = lbfd;
    acc_fd[0].events = POLLIN;

    // getting the server loads
    if ( (ret = get_server_loads(server_loads, servaddr)) == -1) {
        printf("Unable to get server loads\n");
        exit(EXIT_FAILURE);
    }

    counter_time = 5;

    listen(lbfd, 3);

    // now we accept connections
    while (1) {

        // getting the initial time
        time_t t_start = time(NULL);
        struct tm tm_start = *localtime(&t_start);
        int systime_start = tm_start.tm_sec;

        int rv = poll(acc_fd, 1, counter_time * 1000);

        //printf("rv: %d, time_elapsed: %d\n", rv, time_elapsed);

        if (rv > 0) {

            // accepting a new connection
            //printf("[ACCEPT]");
            socklen_t clilen = sizeof(cliaddr);
            clifd = accept(lbfd, (struct sockaddr *) &cliaddr, &clilen);

            //printf("the clifd: %d\n", clifd);

            if (clifd < 0) {
                perror("accept failed, exit");
                exit(EXIT_FAILURE);
            }

            if (fork() == 0) {

                // the child process
                close(lbfd);

                char send_time[20] = "Send Time", recv_buffer[50];

                int server_idx = server_with_min_load(server_loads);

                int num_tries = 0, ret = -1, to_be_sent = strlen(send_time) + 1;
                int send_fd = socket(PF_INET
                                    , SOCK_STREAM
                                    , 0);

                if ( send_fd < 0 ) {
                    perror("socket failed");
                    exit(EXIT_FAILURE);
                }
                int len_sent = 0;

                if ( connect(send_fd
                            , (struct sockaddr *) &servaddr[server_idx]
                            , sizeof(servaddr[server_idx])) < 0 ) {
                                perror("connect failed");
                                exit(EXIT_FAILURE);
                            }

                printf("Sending client request to %s %d\n", inet_ntoa(servaddr[server_idx].sin_addr), server_idx);
                while (ret < 0 && num_tries < 5) {
                    while ( (ret = send(send_fd
                                        , send_time + len_sent
                                        , to_be_sent - len_sent
                                        , 0
                                        )) > 0 ) {
                        // accumulating len_sent
                        len_sent += ret;
                        if (to_be_sent == len_sent) break;
                    }
                }

                if (ret < 0) {
                    perror("send date request");
                    goto close_connection_to_client;
                }

                int len_recvd = 0;
                ret = -1; num_tries = 0;

                //printf("[IN CHILD] recving\n");
                while (ret < 0 && num_tries < 5) {
                    while ( (ret = recv (send_fd
                                        , recv_buffer - len_recvd
                                        , 50 - len_recvd
                                        , 0
                                        )) > 0) {
                        len_recvd += ret;
                        if (recv_buffer[len_recvd - 1] == '\0') break;
                    }
                }

                if (ret < 0) {
                    perror("date recv failed");
                    goto close_connection_to_client;
                }

                //printf("[IN CHILD] sending to child, %s|\n", recv_buffer);
                // sending the date to the client
                len_sent = 0; ret = -1; num_tries = 0;
                to_be_sent = strlen(recv_buffer) + 1;

                while (ret < 0 && num_tries < 5) {
                    while ( (ret = send(clifd
                                        , recv_buffer + len_sent
                                        , to_be_sent - len_sent
                                        , 0
                                        )) > 0) {
                        len_sent += ret;
                        if (len_sent == to_be_sent) break;
                    }
                }

                if (ret < 0) {
                    perror("time send failed");
                    goto close_connection_to_client;
                }

                //printf("SENT\n");

                close(send_fd);
                close(clifd);
                exit(EXIT_SUCCESS);

                close_connection_to_client:
                close(send_fd);
                close(clifd);
                exit(EXIT_FAILURE);
            }

            // getting the final time
            time_t t_end = time(NULL);
            struct tm tm_end = *localtime(&t_end);
            int systime_end = tm_end.tm_sec;

            time_elapsed = systime_end - systime_start;

            // end of the loop
            counter_time -= time_elapsed;
            counter_time = (counter_time < 0) ? 0 : counter_time;

        }
        else {    
            if ( (ret = get_server_loads(server_loads, servaddr)) == -1) {
                printf("Unable to get server loads\n");
                exit(EXIT_FAILURE);
            } else
                counter_time = 5;
        }
    }
}