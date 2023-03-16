#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6969
#define UNAME_SIZE 27  // 25 + 1 (\n) + 1 (\0 added by fgets in the worst case)
#define BUFFER_SIZE 50 // ALL BUFEFRS ARE NO GREATER THAN 50 IN SIZE
#define CMD_BUFFER_SIZE 200
#define DIR_BUF_SIZE (BUFFER_SIZE - 1)

#define SEND_LOG_FILE "server_send_log.txt"
#define RECV_LOG_FILE "server_recv_log.txt"
#define CLIENT_DISCONNECTED "CLIENT DISCONNECTED"
#define FOUND "FOUND"
#define NOT_FOUND "NOT-FOUND"
#define LOGIN_PROMPT "LOGIN:"

int logging = 0;

// UI MACROS

/*
// LOGGING MACROS
#define TO_TEXT(LOG) "%s", LOG
#define BYTES_SENT(NUM_BYTES) "BYTES SENT: %d\n", NUM_BYTES
#define SEND_LOG(SEND_LOG_ENTRY, CLIENT_ADDR)                                                   \
    do                                                                                          \
    {                                                                                           \
        FILE *__send_log_fptr = fopen(SEND_LOG_FILE, "a");                                      \
        if (__send_log_fptr == NULL) perror("[ERROR] fprintf failed"); \
        time_t t = time(NULL);                                                                  \
        struct tm tm = *localtime(&t);                                                          \
        fprintf(__send_log_fptr, "[SEND LOG %d-%02d-%02d %02d:%02d:%02d] [CLIENT %s:%d] ",      \
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, \
                inet_ntoa(CLIENT_ADDR.sin_addr), ntohs(CLIENT_ADDR.sin_port));                  \
        fprintf(__send_log_fptr, SEND_LOG_ENTRY);                                               \
        fprintf(__send_log_fptr, "\n");                                                         \
        fclose(__send_log_fptr);                                                                \
    } while (0)

#define BYTES_RECVD(NUM_BYTES) "BYTES RECVD: %d\n", NUM_BYTES
#define RECV_LOG(RECV_LOG_ENTRY, CLIENT_ADDR)                                                   \
    do                                                                                          \
    {                                                                                           \
        FILE *__recv_log_fptr = fopen(RECV_LOG_FILE, "a");                                      \
        time_t t = time(NULL);                                                                  \
        struct tm tm = *localtime(&t);                                                          \
        fprintf(__recv_log_fptr, "[RECV LOG %d-%02d-%02d %02d:%02d:%02d] [CLIENT %s:%d] ",      \
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, \
                inet_ntoa(CLIENT_ADDR.sin_addr), ntohs(CLIENT_ADDR.sin_port));                  \
        fprintf(__recv_log_fptr, RECV_LOG_ENTRY);                                               \
        fprintf(__recv_log_fptr, "\n");                                                         \
        fclose(__recv_log_fptr);                                                                \
    } while (0)
*/

// SOCKET CREATION UTILITY MACROS
#define SOCKET_ERR_PRMPT "[ERROR SOCKET CREATE] UNABLE TO CREATE SOCKET"
#define SOCKET(SOCK_FD)                            \
    do                                             \
    {                                              \
        SOCK_FD = socket(PF_INET, SOCK_STREAM, 0); \
        if (SOCK_FD < 0)                           \
        {                                          \
            perror(SOCKET_ERR_PRMPT);              \
            exit(EXIT_FAILURE);                    \
        }                                          \
    } while (0)

// SERVER ADDRESS INITIALIZATION MACROS
#define SERVER_ADDR_INIT(SERV_ADDR, IP_INIT) \
    do                                       \
    {                                        \
        SERV_ADDR.sin_addr.s_addr = IP_INIT; \
        SERV_ADDR.sin_port = htons(PORT);    \
        SERV_ADDR.sin_family = AF_INET;      \
    } while (0)

// SERVER BIND UTILITY MACROS
#define BIND_ERR_PRMPT "[ERROR SOCKET BIND] UNABLE TO BIND TO THE LOCAL ADDRESS"
#define BIND(SOCK_FD, SERV_ADDR)                                                 \
    do                                                                           \
    {                                                                            \
        if (bind(SOCK_FD, (struct sockaddr *)&SERV_ADDR, sizeof(SERV_ADDR)) < 0) \
        {                                                                        \
            perror(BIND_ERR_PRMPT);                                              \
            exit(EXIT_FAILURE);                                                  \
        }                                                                        \
    } while (0)

// ACCEPT UTILITY MACROS
#define ACCEPT_ERR_PRMPT "[ERROR ACCEPT] UNABLE TO ACCEPT CONNECTION"
#define ACCEPT(CONN_SOCK_ADDR, CLIENT_ADDR)                                            \
    do                                                                                 \
    {                                                                                  \
        socklen_t __cli_len = sizeof(CLIENT_ADDR);                                     \
        CONN_SOCK_ADDR = accept(sock_fd, (struct sockaddr *)&CLIENT_ADDR, &__cli_len); \
        if (CONN_SOCK_ADDR < 0)                                                        \
        {                                                                              \
            perror(ACCEPT_ERR_PRMPT);                                                  \
            exit(EXIT_FAILURE);                                                        \
        }                                                                              \
    } while (0)

// SEND MACROS
#define SEND_ERR_PRMPT "[ERROR SEND] SEND FAILED"
#define PERSISTENT_SEND(CONN_SOCK_FD, MSG, MSG_SIZE)                                 \
    do                                                                                                  \
    {                                                                                                   \
        int __send_ret = 0, __bytes_sent = 0, __msg_size = 0;                                           \
        __msg_size = (MSG_SIZE) + 1;                                                                    \
        while ((__send_ret = send(CONN_SOCK_FD, MSG + __bytes_sent, __msg_size - __bytes_sent, 0)) > 0) \
        {                                                                                               \
            __bytes_sent += __send_ret;                                                                 \
            if (__bytes_sent == __msg_size)                                                             \
                break;                                                                                  \
        }                                                                                               \
        if (__send_ret < 0) perror(SEND_ERR_PRMPT);                                                                     \
    } while (0)

// RECV MACROS
#define RECV_ERR_PRMPT "[ERROR RECV] RECV FAILED"
#define PERSISTENT_RECV(CONN_SOCK_FD, BUF, POLL)                         \
    do                                                                                                     \
    {                                                                                                      \
        POLL = 0;                                                                                          \
        int __recv_ret = 0, __bytes_recvd = 0;                                                             \
        memset(BUF, 0, BUFFER_SIZE);                                                                       \
        while ((__recv_ret = recv(CONN_SOCK_FD, BUF + __bytes_recvd, BUFFER_SIZE - __bytes_recvd, 0)) > 0) \
        {                                                                                                  \
            __bytes_recvd += __recv_ret;                                                                   \
            if (BUF[__bytes_recvd - 1] == '\0')                                                            \
            {                                                                                              \
                if (__bytes_recvd == BUFFER_SIZE)                                                          \
                    POLL = 1;                                                                              \
                break;                                                                                     \
            }                                                                                              \
        }                                                                                                  \
        if (__recv_ret == 0) return;                                                                       \
        if (__recv_ret < 0) perror(RECV_ERR_PRMPT);                                                        \
    } while (0)

#define EXHAUSTIVE_RECV(CONN_SOCK_FD, RECV_BUF, NET_BUF, POLL) \
    do                                                                                                 \
    {                                                                                                  \
        POLL = 1;                                                                                      \
        memset(RECV_BUF, 0, BUFFER_SIZE);                                                              \
        strcpy(NET_BUF, "");                                                                           \
        while (POLL != 0)                                                                              \
        {                                                                                              \
            POLL = 0;                                                                                  \
            PERSISTENT_RECV(CONN_SOCK_FD, RECV_BUF, POLL);                   \
            strcat(NET_BUF, RECV_BUF);                                                                 \
        }                                                                                              \
    } while (0)

#define EXHAUSTIVE_SEND(CONN_SOCK_FD, SEND_BUF, NET_BUF) \
    do                                                                                  \
    {                                                                                   \
        int __net_str_size = (int)strlen(NET_BUF), __idx = 0;                           \
        while (__net_str_size > 0)                                                      \
        {                                                                               \
            memset(SEND_BUF, 0, BUFFER_SIZE);                                           \
            int i = 0;                                                                  \
            for (i = 0; (i < (BUFFER_SIZE - 1)) && (i < __net_str_size); i++, __idx++)  \
                SEND_BUF[i] = NET_BUF[__idx];                                           \
                                                                                        \
            SEND_BUF[i] = '\0';                                                         \
            PERSISTENT_SEND(CONN_SOCK_FD, SEND_BUF, strlen(SEND_BUF)); \
            if (__net_str_size == (BUFFER_SIZE - 1))                                    \
            {                                                                           \
                char __nul_buf[1] = {0};                                                \
                PERSISTENT_SEND(CONN_SOCK_FD, __nul_buf, 1);           \
            }                                                                           \
            __net_str_size -= i;                                                        \
        }                                                                               \
    } while (0)

#define CHECK_IF_CONN_CLOSED(CONN_SOCK_FD, BUF)                   \
    do                                                            \
    {                                                             \
        int __recv_ret = recv(CONN_SOCK_FD, BUF, BUFFER_SIZE, 0); \
        if (__recv_ret == 0)                                      \
            return;                                               \
    } while (0)

int username_in_file(char *username)
{

    // OPENING FILES
    FILE *uame_file = fopen("users.txt", "r"); // OPENING FILE IN READ MODE FOR READING

    // SETTING UP VARIABLES
    char retrieved_name[UNAME_SIZE];
    char *fgets_check;

    // TAKING INPUT LINE BY LINE
    while ((fgets_check = fgets(retrieved_name, UNAME_SIZE, uame_file)) != NULL)
    {
        //  CHECKING THE LENGTH OF THE STRING READ
        int retrieved_name_len = strlen(retrieved_name);

        // DEALING WITH THE \n AT THE END
        if (retrieved_name[retrieved_name_len - 1] == '\n')
            retrieved_name[retrieved_name_len - 1] = '\0';
        else
            break; // DEALING WHEN THE STRING IS TOO BIG TO BE USERNAME

        // STRCMP TO CHECK
        if (strcmp(username, retrieved_name) == 0)
            return 1;
    }

    // NO MATCH, HENCE USERNAME NOT IN FILE
    return 0;
}

#define HANDLE_ERR_PRMPT "[ERROR HANDLING]"
#define HANDLE_ERR(CONN_SOCK_FD)  \
    do                                                    \
    {                                                     \
        perror(HANDLE_ERR_PRMPT);                         \
        handle_err(CONN_SOCK_FD); \
    } while (0)

void handle_err(int conn_sock_fd)
{
    char send_buffer[5] = "####";
    PERSISTENT_SEND(conn_sock_fd, send_buffer, strlen(send_buffer));
}

void handle_dir(int conn_sock_fd, char *cmd_args)
{
    // HANDLING THE DIRECTORY NAME TOO BIG CASE
    char *fname;
    char pwd[CMD_BUFFER_SIZE], buffer[BUFFER_SIZE];

    if (cmd_args == NULL)
    {
        getcwd(pwd, CMD_BUFFER_SIZE);
        // ERROR CHECK
        if (pwd == NULL)
        {
            HANDLE_ERR(conn_sock_fd);
            return;
        }
    }
    else
        strcpy(pwd, cmd_args);

    DIR *dir;                       // DESCRIPTOR USED TO OPEN AND HANDLE THE DIRECTORY
    struct dirent *directory_entry; // USED TO ITERATE OVER THE DIRECTORY AND ENUMERATE FILES


    if ((dir = opendir(pwd)) == NULL)
    { // IF THE DIRECTORY OPENING FAILS
        HANDLE_ERR(conn_sock_fd);
        return;
    }

    int __size, __last_send, __byte_count = 0, __first_file = 1;
    while ((directory_entry = readdir(dir)) != NULL)
    {

        fname = directory_entry->d_name;
        if (fname[0] != '.')
        { // NOT A HIDDEN FILE

            __size = strlen(fname);
            for (int _byte = 0; _byte < __size; _byte++, __byte_count++)
            {
                // TO CHECK IF THE CURRENT BYTE FITS IN THE CURRENT BUFFER

                if (__byte_count < (BUFFER_SIZE - 1))
                {

                    if ( (_byte == 0) && (__first_file == 0) )
                        buffer[__byte_count ++] = ' '; // SPACE AT THE END OF EACH FILE
                    
                    buffer[__byte_count] = fname[_byte];
                }
                else
                {
                    buffer[BUFFER_SIZE - 1] = '\0';
                    PERSISTENT_SEND(conn_sock_fd, buffer, BUFFER_SIZE - 1);

                    __last_send = __byte_count;

                    __byte_count = -1; // BUFFER EMPTIED
                    _byte--;
                }
            }
            __first_file = 0;
        }
    }

    if ( ( __byte_count == 0 ) && ( __last_send == ( BUFFER_SIZE - 1 ) ) ) { char nul_buf[1] = {0}; PERSISTENT_SEND(conn_sock_fd, nul_buf, strlen(nul_buf)); }

    if (__byte_count)
    {
        buffer[__byte_count] = '\0';
        PERSISTENT_SEND(conn_sock_fd, buffer, __byte_count);
        if ( __byte_count == ( BUFFER_SIZE - 1 ) ) { char nul_buf[1] = {0}; PERSISTENT_SEND(conn_sock_fd, nul_buf, strlen(nul_buf)); }
    }

    closedir(dir);

    return;
}

void handle_pwd(int conn_sock_fd)
{

    char send_buffer[BUFFER_SIZE], cwd[CMD_BUFFER_SIZE];

    if ((getcwd(cwd, CMD_BUFFER_SIZE)) == NULL)
    {
        HANDLE_ERR(conn_sock_fd);
    }
    else
        EXHAUSTIVE_SEND(conn_sock_fd, send_buffer, cwd);
}

void handle_invalid(int conn_sock_fd)
{
    char send_buffer[5] = "$$$$";
    PERSISTENT_SEND(conn_sock_fd, send_buffer, strlen(send_buffer));
}

int parse_instruction(char *recvd, char *cmd, char *cmd_args)
{
    int __cnt = 0;
    char *token = strtok(recvd, " ");
    do
    {
        __cnt++;
        if (__cnt == 1)
            strcpy(cmd, token);
        else if (__cnt == 2)
        {
            strcpy(cmd_args, token);
        }
    } while ((token = strtok(NULL, " ")));
    return (__cnt > 1);
}

void handle_client(int conn_sock_fd, char *send_buffer, char *recv_buffer, struct sockaddr_in clientaddr)
{

    int poll = 0;

    // SENDING THE LOGIN MESSAGE
    char login_msg[BUFFER_SIZE] = LOGIN_PROMPT;
    PERSISTENT_SEND(conn_sock_fd, login_msg, strlen(login_msg));

    // THEN WAITS FOR RECEIVING THE USERNAME
    char username[BUFFER_SIZE];
    PERSISTENT_RECV(conn_sock_fd, username, poll);

    // CHECKING IF USERNAME IS IN A FILE
    int uname_in_file = 0;
    if ((uname_in_file = username_in_file(username)))
    {
        strcpy(send_buffer, FOUND);
        PERSISTENT_SEND(conn_sock_fd, send_buffer, strlen(send_buffer));
    }
    else
    {
        strcpy(send_buffer, NOT_FOUND);
        PERSISTENT_SEND(conn_sock_fd, send_buffer, strlen(send_buffer));
        CHECK_IF_CONN_CLOSED(conn_sock_fd, recv_buffer);

        return;
    }

    while (1)
    {

        // BUFFERS INITIALIZED
        char instruction_buffer[CMD_BUFFER_SIZE];
        char cmd[BUFFER_SIZE];
        char cmd_args[CMD_BUFFER_SIZE];

        // RECV'ING COMMANDS FROM CLIENT SIDE
        EXHAUSTIVE_RECV(conn_sock_fd, recv_buffer, instruction_buffer, poll);

        // PARSING THE INSTRUCTION
        int has_args = parse_instruction(instruction_buffer, cmd, cmd_args);

        if (strcmp(cmd, "pwd") == 0)
        {
            printf("[COMD LOG] pwd\n");
            if (strcmp(instruction_buffer + 3, "") != 0)
            {
                handle_invalid(conn_sock_fd);
                continue;
            }
            handle_pwd(conn_sock_fd);
        }

        // HANDLING "dir"
        else if (strcmp(cmd, "dir") == 0)
        {
            printf("[COMD LOG] dir\n");
            if (has_args)
                handle_dir(conn_sock_fd, cmd_args);
            else
                handle_dir(conn_sock_fd, NULL);
        }

        // HANDLING "cd"
        else if (strcmp(cmd, "cd") == 0)
        {
            printf("[COMD LOG] cd\n");
            if (has_args == 0)
                HANDLE_ERR(conn_sock_fd);
            else
            {
                int __ret_chdir = chdir(cmd_args);
                char send_cd_res[BUFFER_SIZE];
                if (__ret_chdir == 0)
                    EXHAUSTIVE_SEND(conn_sock_fd, send_cd_res, cmd_args);
                else
                    HANDLE_ERR(conn_sock_fd);
            }
        }

        // INVALID COMMAND
        else {
            printf("[COMD LOG] %s\n", cmd);
            printf("[INVALID HANDLING]: NO SUCH COMMAND\n");
            handle_invalid(conn_sock_fd);
        }
    }
}

int main()
{
    int sock_fd, conn_sock_fd, pid;

    struct sockaddr_in servaddr, clientaddr;

    // SOCKET CREATE
    SOCKET(sock_fd);

    // SERVER ADDRESS INIT
    SERVER_ADDR_INIT(servaddr, INADDR_ANY);

    // SOCKET BIND
    BIND(sock_fd, servaddr);

    // SOCKET LISTEN WITH 10 BACKLOG
    listen(sock_fd, 10);

    while (1)
    {

        // ACCEPTING CLIENT CONNECTIONS
        ACCEPT(conn_sock_fd, clientaddr);
        printf("[CONNECTED@%s:%d]\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // FORKING TO LET THE CHILD PROCESS HANDLE THE CONNECTION
        if ((pid = fork()) == 0)
        {

            // CLOSING THE CURRENT LISTENING SOCKET FOR THE CHILD PROCESS
            close(sock_fd);

            // UTILITY BUFFERS
            char *send_buffer = (char *)malloc(BUFFER_SIZE);
            char *recv_buffer = (char *)malloc(BUFFER_SIZE);

            // HANDLING THE CLIENT IN A UTILITY FUNCTION
            handle_client(conn_sock_fd, send_buffer, recv_buffer, clientaddr);

            free(send_buffer);
            free(recv_buffer);

            printf("[CONNECTION CLOSED@%s:%d]\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
            // EXITING FROM THE CHILD PROCESS
            exit(EXIT_SUCCESS);
        }

        // CLOSING THE NEW DESCRIPTOR FOR THE PARENT PROCESS WHICH WILL
        // CONTINUE LISTENING FOR MORE CONNECTIONS
        close(conn_sock_fd);
    }
}