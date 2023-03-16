#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DIR_SZIE 22
int main () {
    char dir[DIR_SZIE];
    int i = 0;
    do {
        if (i < 2) {
            i ++; printf("i:%d\n", i); continue;
        }
    } while (0);

    if ( getcwd(dir, sizeof(dir)) == NULL) printf("FAILED@%d\n", DIR_SZIE);

    char try[50] = "  cd        /home/yatindra";
    char cmd[50] = "";
    memset(cmd, 0, 50);
    char *token = token = strtok(try, " ");
    do {
        //printf("token: %s\n", token);
        strcat(cmd, token);
        strcat(cmd, " ");
    } while (( token = strtok(NULL, " ") ) );

    printf("cmd: [%s]\n", cmd);
}