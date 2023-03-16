#include "mysocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    int sock_fd = my_socket(AF_INET, SOCK_MyTCP, 0);

    printf("SEE SOCKFD: %d\n", sock_fd);

    struct sockaddr_in addr, cliaddr;

    addr.sin_port = 6969;
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    my_bind(sock_fd, (struct sockaddr *) &addr, sizeof(addr));

    my_listen(sock_fd, 5);

    socklen_t clilen = sizeof(cliaddr);
    int clifd = my_accept(sock_fd, (struct sockaddr *) &cliaddr, &clilen);

    if (clifd < 0) {
        printf("NOOOOO!!\n");
    }


    const char *msg1 = "start0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100010001000100010001000100010001000100010001000100010001000100010001000100010001000100010001000002000200020002000200020002000200020002000200020002000200020002000200020002000200020002000200020000030003000300030003000300030003000300030003000300030003000300030003000300030003000300030003000300000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000005000500050005000500050005000500050005000500050005000500050005000500050005000500050005000500050000060006000600060006000600060006000600060006000600060006000600060006000600060006000600060006000600000700070007000700070007000700070007000700070007000700070007000700070007000700070007000700070007000008000800080008000800080008000800080008000800080008000800080008000800080008000800080008000800080000090009000900090009000900090009000900090009000900090009000900090009000900090009000900090009000900000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a00b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100010001000100010001000100010001000100010001000100010001000100010001000100010001000100010001000002000200020002000200020002000200020002000200020002000200020002000200020002000200020002000200020000030003000300030003000300030003000300030003000300030003000300030003000300030003000300030003000300000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000005000500050005000500050005000500050005000500050005000500050005000500050005000500050005000500050000060006000600060006000600060006000600060006000600060006000600060006000600060006000600060006000600000700070007000700070007000700070007000700070007000700070007000700070007000700070007000700070007000008000800080008000800080008000800080008000800080008000800080008000800080008000800080008000800080000090009000900090009000900090009000900090009000900090009000900090009000900090009000900090009000900000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a00b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100010001000100010001000100010001000100010001000100010001000100010001000100010001000100010001000002000200020002000200020002000200020002000200020002000200020002000200020002000200020002000200020000030003000300030003000300030003000300030003000300030003000300030003000300030003000300030003000300000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000005000500050005000500050005000500050005000500050005000500050005000500050005000500050005000500050000060006000600060006000600060006000600060006000600060006000600060006000600060006000600060006000600000700070007000700070007000700070007000700070007000700070007000700070007000700070007000700070007000008000800080008000800080008000800080008000800080008000800080008000800080008000800080008000800080000090009000900090009000900090009000900090009000900090009000900090009000900090009000900090009000900000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a00b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100010001000100010001000100010001000100010001000100010001000100010001000100010001000100010001000002000200020002000200020002000200020002000200020002000200020002000200020002000200020002000200020000030003000300030003000300030003000300030003000300030003000300030003000300030003000300030003000300000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000005000500050005000500050005000500050005000500050005000500050005000500050005000500050005000500050000060006000600060006000600060006000600060006000600060006000600060006000600060006000600060006000600000700070007000700070007000700070007000700070007000700070007000700070007000700070007000700070007000008000800080008000800080008000800080008000800080008000800080008000800080008000800080008000800080000090009000900090009000900090009000900090009000900090009000900090009000900090009000900090009000900000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a000a00b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000b000bendend";
    const char *msg2 = "2I'm up!";
    const char *msg3 = "3I'm up!";
    const char *msg4 = "4I'm up!";
    size_t msg_size = strlen(msg1);
    
    char buf[700];

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg1, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg2, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg3, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_send(clifd, msg4, msg_size + 1, 0);

    my_recv(clifd, buf, 700, 0);
    //printf("[IN SERVER]buf: %s\n", buf);

    my_close(clifd);
    my_close(sock_fd);
}