#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//NUMERICAL CONSTANTS
#define SERVER_PORT htons(2000)
#define BUF_SIZE 20
#define UIO_SIZE (BUF_SIZE - 1) // DEFINED TO ALLOW FOR fgets BEHAVIOUR OF TAKING TILL n - 1
#define RECV_BUF_SIZE 100
#define SEND_BUF_SIZE 100
#define BACKLOG 5
#define STCK_EMPT -1

// ERROR HANDLING MACROS
#define SOCK_CREATE_ERR do { \
                            perror("[ERROR] UNABLE TO OPEN SOCKET "); \
                            exit(EXIT_FAILURE); \
                            } while (0)
#define BIND_ERR do { \
                        perror("[ERROR] UNABLE TO BIND TO LOCAL ADDR "); \
                        exit(EXIT_FAILURE); \
                        } while (0)
#define CONN_ACCEPT_ERR do { \
                        perror("[ERROR] UNABLE TO ACCEPT CONNECTION "); \
                        exit(EXIT_FAILURE); \
                        } while (0)

#define FREAD_ERR(STREAM) do { \
                        if (ferror(STREAM)) { \
                            perror("[ERROR] FILE READ FAILED "); \
                            clearerr(STREAM); \
                            exit(EXIT_FAILURE); \
                        } \
                    } while (0)

#define MALLOC_ERR do { \
                        perror("[ERROR] MALLOC FAILED "); \
                        exit(EXIT_FAILURE); \
                        } while(0)
#define REALLOC_ERR do { \
                        perror("[ERROR] REALLOC FAILED "); \
                        exit(EXIT_FAILURE); \
                        } while(0)

#define VAR_RECV_ERR(FLAG) do { \
                                if (FLAG) printf("[IMP] ALL INPUT RECEIVED\n"); \
                                else printf("[IMP] ERROR, DISCONNECTD BEFORE INPUT FINISH\n"); \
                            } while (0)

// SEND-RECV MACROS
#define SEND(SOCK_FD,MSG) do { \
                                int __bytes_sent = 0; \
                                while (1) { \
                                    __bytes_sent += send(SOCK_FD, (MSG) + __bytes_sent, (strlen(MSG) + 1) - __bytes_sent, 0); \
                                    if (__bytes_sent == (strlen(MSG) + 1)) break; \
                                } \
                            } while (0)

#define RECV(SOCK_FD,BUF) recv(SOCK_FD, BUF, RECV_BUF_SIZE, 0)

// INITIALIZE SERVER ADDR
#define INIT_SERV_ADDR(SERV_ADDR) do { \
                                        SERV_ADDR.sin_family = AF_INET; \
                                        SERV_ADDR.sin_addr.s_addr = INADDR_ANY; \
                                        SERV_ADDR.sin_port = SERVER_PORT; \
                                    } while (0)

// BIND MACRO
#define BIND(SOCK_FD,SERV_ADDR_IN) do { \
                                        int __bind_res = bind(SOCK_FD, (struct sockaddr *) &SERV_ADDR_IN, sizeof(SERV_ADDR_IN)); \
                                        if (__bind_res < 0) BIND_ERR; \
                                    } while (0)

// ACCEPT MACRO
#define ACCEPT(SOCK_FD,CLI_ADDR,CONN_SOCK_FD) do { \
                                                    socklen_t __cli_len = sizeof(CLI_ADDR); \
                                                    CONN_SOCK_FD = accept(SOCK_FD, (struct sockaddr *) &CLI_ADDR, &__cli_len); \
                                                    if (CONN_SOCK_FD < 0) CONN_ACCEPT_ERR; \
                                                } while (0)

// malloc MACRO
#define MALLOC_SAFE(TYPE,NUM,PTR) do { \
                                        TYPE *__tmp = (TYPE *) malloc ((NUM) * sizeof (TYPE)); \
                                        if (__tmp == NULL) MALLOC_ERR; \
                                        else PTR = __tmp; \
                                        } while(0)

// realloc MACRO
#define REALLOC_SAFE(TYPE,NUM,INIT_PTR) do { \
                                        void *__tmp = realloc(INIT_PTR,(NUM) * sizeof(TYPE)); \
                                        if (__tmp == NULL) REALLOC_ERR; \
                                        else INIT_PTR = __tmp; \
                                        } while (0)

char *variable_length_recv(int conn_sock_fd, int *flag) {

    printf("[LOG] variable_length_input commence\n");
    char tmp_buf[BUF_SIZE];
    memset(tmp_buf, 0, BUF_SIZE);

    int current_buf_size = 0;
    
    // malloc'ing the buffer
    char *buf;
    int res;
    MALLOC_SAFE(char, BUF_SIZE, buf);
    printf("[LOG] malloc'd buf@%p\n", buf);

    current_buf_size += BUF_SIZE;
    memset(buf, 0, BUF_SIZE); // Empty string to initialize the buffer

    printf("[LOG] memset, and malloc'd buf@%p, and current_buf_size is %d\n", buf, current_buf_size);
    *flag = 0; // Setting flag to 0, if the below loop gets terminated due to client disconnect 

    while ((res = recv(conn_sock_fd, tmp_buf, UIO_SIZE, 0)) != 0) {
        // CHECK IF BUFFER IS NOT COMPLETELY FULL

        /*
            Note this also includes the '\n'
            Example: strlen("CAFE\n") == 5; (and not 4)
            Hence the '\n' is at the index "strlen - 1"
        */

        tmp_buf[res] = '\0'; // NULL terminating the string
        //int offset = strlen(tmp_buf);
        printf("[LOG] offset is %d\n", res);

        // Adding the strings
        strcat(buf, tmp_buf);
        printf("[LOG] Concatenation done, buf is %lu bytes\n", strlen(buf));

        // The read is done
        if (tmp_buf[res - 1] == '\0') {
            *flag = 1;
            break;
        }

        // realloc for increasing the "buf" size
        current_buf_size += BUF_SIZE;
        REALLOC_SAFE(char, current_buf_size, buf);
        printf("[LOG] realloc'd@%p, and current_buf_size is %d\n", buf, current_buf_size);
    }

    return buf;
}

#define STCK_EMPT -1

typedef struct double_node {
    double value;
    struct double_node *next;
} stack_double;

//typedef stack_double *abs_stack_double;

int is_empty_double(stack_double *stck) {
    return stck == NULL;
}

double top_double (stack_double **stck) {
    stack_double *stck_ptr = *stck;

    if (stck_ptr == NULL) {
        printf("[WARN] stack empty, can't top\n");
        return STCK_EMPT;     
    }

    return stck_ptr -> value;
}

/*
stack_double *create_stack_double() {
    stack_double *mem = (stack_double *) malloc (sizeof (stack_double));
    if (mem == NULL) printf("[WARN] malloc failed\n");
    mem -> next = NULL;
    return mem;
}*/

stack_double *create_stack_double() {return NULL;}

void push_double (stack_double **stck, double value) {
    stack_double *stck_ptr = *stck;
    stack_double *allocated_ptr = (stack_double *) malloc(sizeof (stack_double));
    *stck = allocated_ptr;
    allocated_ptr -> value = value;
    allocated_ptr -> next = stck_ptr;
}

double pop_double (stack_double **stck) {
    stack_double *stck_ptr = *stck;

    if (stck_ptr == NULL) {
        printf("[WARN] stack empty, can't pop\n");
        return STCK_EMPT;     
    }

    *stck = (stck_ptr -> next);

    double ret_val = stck_ptr -> value;
    free(stck_ptr);
    return ret_val;
}

// CHAR STACK

typedef struct char_node {
    char value;
    struct char_node *next;
} stack_char;

//typedef stack_double *abs_stack_double;

char top_char (stack_char **stck) {
    stack_char *stck_ptr = *stck;

    if (stck_ptr == NULL) {
        printf("[WARN] stack empty, can't top\n");
        return STCK_EMPT;     
    }

    return stck_ptr -> value;
}

int is_empty_char(stack_char *stck) {
    return stck == NULL;
}

stack_char *create_stack_char() {return NULL;}

void push_char (stack_char **stck, char value) {
    stack_char *stck_ptr = *stck;
    stack_char *allocated_ptr = (stack_char *) malloc(sizeof (stack_char));
    *stck = allocated_ptr;
    allocated_ptr -> value = value;
    allocated_ptr -> next = stck_ptr;
}

char pop_char (stack_char **stck) {
    stack_char *stck_ptr = *stck;

    if (stck_ptr == NULL) {
        printf("[WARN] stack empty, can't pop\n");
        return STCK_EMPT;     
    }

    *stck = (stck_ptr -> next);

    char ret_val = stck_ptr -> value;
    free(stck_ptr);
    return ret_val;
}

int isop(char tok) {
    return (tok == '+') || (tok == '-') || (tok == '*') || (tok == '/');
}

double eval(double op1, double op2, char op) {
    if (op == '-') return op1 - op2;
    if (op == '+') return op1 + op2;
    if (op == '/') return op1 / op2;
    if (op == '*') return op1 * op2;
    printf("[ERROR] NO MATCHING OPERATION TO %c!\n", op);
    exit(EXIT_FAILURE);
}

int precedence(char op) {
    if (op == '*' || op == '/') return 2;
    else if (op == '+' || op == '-') return 1;
    else return 0;
}

double expression_value(char *expr) {

    stack_double *stck_dbl = create_stack_double();
    stack_char *stck_chr = create_stack_char();
    char tok;
    int i = 0;
    while ((tok = expr[i]) != '\0') {

        printf("[_TOK] %c\n", tok);

        if (isspace(tok)) { /* do nothing */}
    
        else if (isdigit(tok)) {
            char *endptr, *addr = expr + i;
            endptr = addr; // making sure non zero
            double num = strtod(addr, &endptr);
            // The new i
            i = ((int) endptr - (int) expr);

            printf("[ISDIGIT_LOG] num: %lf, i: %d\n", num, i);
            push_double(&stck_dbl, num);

            //continue;
            i --;
        } 
        
        else if (isop(tok)) {
            char op;
            printf("[DEBUG] before isop(tok) loop, is_empty: %d\n", is_empty_char(stck_chr));
            while (!is_empty_char(stck_chr) && 
            (precedence(top_char(&stck_chr)) >= precedence(tok))) {
                printf("[DEBUG] in isop(tok), tok: %c\n", tok);
                double op2 = pop_double(&stck_dbl), op1 = pop_double(&stck_dbl);                
                op = pop_char(&stck_chr);
                printf("[DEBUG] op is %c\n", op);
                push_double(&stck_dbl, eval(op1, op2, op));
            }
            push_char(&stck_chr, tok);
            printf("[DEBUG] Verifying the push, tok: %c, top: %c\n", tok, top_char(&stck_chr));
            //i ++;
        }
        
        else if (tok == '(') {
            printf("[DEBUG] before '(', is_empty: %d\n", is_empty_char(stck_chr));
            push_char(&stck_chr, tok);
            printf("[DEBUG] after '(', is_empty: %d\n", is_empty_char(stck_chr));
        }
        
        else if (tok == ')') {
            char char_stack_top;
            printf("[DEBUG] before tok == ')' loop, is_empty: %d\n", is_empty_char(stck_chr));
            while (!is_empty_char(stck_chr) && ((char_stack_top = top_char(&stck_chr)) != '(')) {
                printf("[DEBUG] in tok == ')', tok: %c\n", tok);
                double op2 = pop_double(&stck_dbl), op1 = pop_double(&stck_dbl);
                char_stack_top = pop_char(&stck_chr);
                push_double(&stck_dbl, eval(op1, op2, char_stack_top));
            }
            if (!is_empty_char(stck_chr)) pop_char(&stck_chr);
        }
        i ++;
    }

    while (!is_empty_char(stck_chr)) {
        printf("[DEBUG] IN THE FINAL POPS\n");
        double op2 = pop_double(&stck_dbl), op1 = pop_double(&stck_dbl);
        char op = pop_char(&stck_chr);
        push_double(&stck_dbl, eval(op1, op2, op));
    }


    //pop_double(&stck_dbl); 
    //pop_double(&stck_dbl);
    double res = pop_double(&stck_dbl);
    printf("[CONFIRM] is_empty: %d\n", is_empty_double(stck_dbl));
    return res;
}

int main () {

    // THE DESCRIPTORS
    int sock_fd, conn_sock_fd;
    // THE SOCKET ADDRESSES
    struct sockaddr_in serv_addr, cli_addr;

    // CREATING SOCKET
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) SOCK_CREATE_ERR;

    // CREATING STRUCT OF ADDRESSES
    INIT_SERV_ADDR(serv_addr);

    // BINDING TO THE ADDRESS
    BIND(sock_fd, serv_addr);

    // LISTENING
    listen(sock_fd, BACKLOG);

    while (1) {
        // ACCEPTING THE CONNECTION AND SAVING SOCKET DESC. IN conn_sock_fd
        ACCEPT(sock_fd, cli_addr, conn_sock_fd);

        // SEDING THE INPUT STRING
        int flag;
        char *expr = variable_length_recv(conn_sock_fd, &flag);

        // CHECKING IF WE RECEIVED EVERYTHING
        VAR_RECV_ERR(flag);

        printf("[LOG] String received: %s, sizeof string: %ld\n", expr, strlen(expr));

        double ans_val = expression_value(expr);
        char ans_to_be_sent[BUF_SIZE];
        sprintf(ans_to_be_sent, "%lf", ans_val);
        printf("[ANS] %lf\n", ans_val);
        
        // SENDING BACK THE ANSWER
        send(conn_sock_fd, ans_to_be_sent, strlen(ans_to_be_sent) + 1, 0);
        //printf("[FINAL] VALUE SENT %d\n", ntohl(expr_val_send));

        // CLOSING THE CONNECTION
        close(conn_sock_fd);
        printf("[FINAL] DONE!\n");
    }
    return 0;
}