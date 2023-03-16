#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
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
    int sock_fd;
    struct sockaddr_in servaddr;
    char recv_buffer[50];

    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { //Error check
        perror("socket"); exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    if (connect(sock_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect failed");
        exit(EXIT_SUCCESS);
    }

    int num_tries = 0, ret = -1;
    // we now go on to recv

    ret = -1; num_tries = 0;
    int len_recvd = 0;
    size_t to_be_recvd = sizeof(recv_buffer);

    while (ret < 0 && num_tries < 5) {
        while ( (ret = recv(sock_fd
                            , recv_buffer + len_recvd
                            , to_be_recvd - len_recvd
                            , 0
                            )) > 0 ) {
            len_recvd += ret;
            if (recv_buffer[len_recvd - 1] == '\0') break;
        }
        num_tries ++;
    }

    // checkig if recv worked successfully
    if (ret < 0) {
        printf("recv timed out, closing and exiting\n");
        goto close_and_failed_exit;
    }
    printf("%s", recv_buffer);

    // successful exit
    close(sock_fd);
    exit(EXIT_SUCCESS);
    
    close_and_failed_exit:
    close(sock_fd);
    exit(EXIT_FAILURE);
}