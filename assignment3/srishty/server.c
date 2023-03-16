#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main (int argc, char **argv) {

    if (argc != 2) {
        printf("Error, incorrect number of commandline arguments\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int sock_fd, conn_sock_fd;
    struct sockaddr_in servaddr, clientaddr;
    
    char recv_buffer[20];
    char send_load[20] = "Send Load", send_time[20] = "Send Time";

    sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    if (sock_fd < 0) {
        perror("Socket creation");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    if ( bind(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(sock_fd, 5);

    while (1) {
        int load = htons(rand() % 101);

        socklen_t clilen = sizeof(clientaddr);
        conn_sock_fd = accept(sock_fd, (struct sockaddr *) &clientaddr, &clilen);

        struct pollfd rcvfd[1];

        while ( 1 ) {

            rcvfd[0].fd = conn_sock_fd;
            rcvfd[0].events = POLLIN | POLLHUP; // polling for input and other 
                                                // side hung up (POLLHUP)

            // recv'ing on what the client wants
            // polling for the same

            int recv_poll = poll(rcvfd, 1, 1000);

            if (recv_poll == 0) {
                printf("recv timedout\n");
                continue;
            }
            if (rcvfd[0].revents & POLLHUP) {
                printf("client disconnected\n");
                break;
            }

            int num_tries = 0, len_recvd = 0, ret = -1;

            while (ret < 0 && num_tries < 5) {
                while ( (ret = recv(conn_sock_fd
                                    , recv_buffer + len_recvd
                                    , 20 - len_recvd
                                    , 0
                                    )) > 0 ) {
                    len_recvd += ret;
                    if (recv_buffer[len_recvd - 1]
                         == '\0') 
                        break;
                }
                num_tries ++;
            }

            // recv failed
            if (ret < 0) {
                perror("failed to recv command\n");
                goto close_and_break;
            }

            if ( strcmp(send_load, recv_buffer) == 0 ) {

                // sending the load
                num_tries = 0; ret = -1;

                // trying to resend in case of failures
                while (ret < 0 && num_tries < 5) {
                    if ( (ret = send(conn_sock_fd
                                    , &load
                                    , sizeof(load)
                                    , 0
                                    )) > 0)//assuming that send works for an integer, per Prof. AG's guidelines
                        break;

                    num_tries ++;
                }

                if (ret < 0) {
                    perror("failed to send load\n");
                    goto close_and_break;
                }

                // printing the message
                printf("Load sent %d\n", ntohs(load) );

            } else if ( strcmp(send_time, recv_buffer) == 0 ) {

                // sending the load

                char datetime[50];            
                memset(datetime, 0, 50);
		        time_t t = time(NULL); struct tm tm = *localtime(&t);

                int len_send = 0, to_be_sent = sprintf(datetime, "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec) + 1;
                num_tries = 0; ret = -1;

                datetime[to_be_sent] = '\0';

                // trying to resend in case of failures
                //printf("SENDING TIME\n");
                while (ret < 0 && num_tries < 5) {
                    while ( (ret = send(conn_sock_fd
                                    , datetime + len_send
                                    , to_be_sent - len_send
                                    , 0
                                    )) > 0) {
                        len_send += ret;
                        if (len_send == to_be_sent) break;
                    }
                    num_tries ++;
                }

                if (ret < 0) {
                    perror("failed to send time\n");
                    goto close_and_break;
                }

                printf("Time sent\n");
            } else {
                printf("Invlaid send request recvd, disconnecting\n");
                goto close_and_break;
            }

            continue;

            close_and_break:
            close(conn_sock_fd);
            break;
        }
    }

    exit(EXIT_SUCCESS);
}