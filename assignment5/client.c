#include "mysocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    int sock_fd = my_socket(AF_INET, SOCK_MyTCP, 0);

    struct sockaddr_in addr;
    size_t len = sizeof(addr);

    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = 6969;
    addr.sin_family = AF_INET;

    if (my_connect(sock_fd, (struct sockaddr *) &addr, len) < 0)
        perror("OH NOOOOO!!!!!!");

    const char *msg1 = "1I gotchaaa!";
    const char *msg2 = "2I gotchaaa!";
    const char *msg3 = "3I gotchaaa!";
    const char *msg4 = "4I gotchaaa!";
    size_t msg_size = strlen(msg1);
    
    char buf[5000];

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg1, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg2, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg3, msg_size + 1, 0);

    my_recv(sock_fd, buf, 50, 0);
    printf("[IN SERVER]buf: %s\n", buf);
    my_send(sock_fd, msg4, msg_size + 1, 0);

    my_close(sock_fd);
}