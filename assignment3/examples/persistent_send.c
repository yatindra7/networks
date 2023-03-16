 * ~ #include <stdio.h>
 * ~ #include <sys/socket.h>
 * ~ 
 * ~ #define MSG_SIZE 25
 * ~ 
 * ~ int main () {
 * ~    int sock_fd = socket(PF_INET, SOCK_STREAM, 0); // TCP Socket
 * ~    if (sock_fd) {
 * ~        perror("socket");
 * ~        exit(EXIT_SUCCESS);
 * ~    }
 * ~    char message[MSG_SIZE] = "This is persistent";
 * ~    persistent_send(sock_fd, message, strlen(message) + 1); // NULL
 * ~    return 0;
 * ~ 