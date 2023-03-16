#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 20
#define UIO_SIZE (BUF_SIZE - 1)

#define MALLOC_ERR do { \
                        perror("[ERROR] MALLOC FAILED "); \
                        exit(EXIT_FAILURE); \
                        } while(0)
#define REALLOC_ERR do { \
                        perror("[ERROR] REALLOC FAILED "); \
                        exit(EXIT_FAILURE); \
                        } while(0)

// malloc MACRO
#define MALLOC_SAFE(TYPE,NUM,PTR) do { \
                                        TYPE *_tmp = (TYPE *) malloc ((NUM) * sizeof (TYPE)); \
                                        if (_tmp == NULL) MALLOC_ERR; \
                                        else PTR = _tmp; \
                                        } while(0)

// realloc MACRO
#define REALLOC_SAFE(TYPE,NUM,INIT_PTR) do { \
                                        void *_tmp = realloc(INIT_PTR,(NUM) * sizeof(TYPE)); \
                                        if (_tmp == NULL) REALLOC_ERR; \
                                        else INIT_PTR = _tmp; \
                                        } while (0)

char *variable_length_input() {

    printf("[LOG] variable_length_input commence\n");
    char tmp_buf[BUF_SIZE];
    int current_buf_size = 0;
    
    // malloc'ing the buffer
    char *buf;
    char *res;
    MALLOC_SAFE(char, BUF_SIZE, buf);
    printf("[LOG] malloc'd buf@%p", buf);

    current_buf_size += BUF_SIZE;
    memset(buf, 0, BUF_SIZE); // Empty string to initialize the buffer

    printf("[LOG] memset, and malloc'd buf@%p, and current_buf_size is %d\n", buf, current_buf_size);

    while ((res = fgets(tmp_buf, BUF_SIZE, stdin)) != NULL) {
        // CHECK IF BUFFER IS NOT COMPLETELY FULL

        /*
            Note this also includes the '\n'
            Example: strlen("CAFE\n") == 5; (and not 4)
            Hence the '\n' is at the index "strlen - 1"
        */
        int offset = strlen(tmp_buf);
        printf("[LOG] offset is %d\n", offset);

        // Adding the strings
        strcat(buf, tmp_buf);
        printf("[LOG] Concatenation done, buf is %lu bytes\n", strlen(buf));

        // The read is done
        if (offset != UIO_SIZE) {
            int end = (((int)strlen(buf)) - 1);
            buf[end] = '\0';
            break;
        }

        // realloc for increasing the "buf" size
        current_buf_size += BUF_SIZE;
        REALLOC_SAFE(char, current_buf_size, buf);
        printf("[LOG] realloc'd@%p, and current_buf_size is %d\n", buf, current_buf_size);
    }

    return buf;
}

int main() {
    printf("[LOG] main commence\n");
    char *in_buf = variable_length_input();
    printf("[INP] %s\n", in_buf);
    return 0;
}

/* double pop_double_stack(stack_double *stck) {
    stack_double *top, *sec_top;
    sec_top = stck;

    if (sec_top == NULL) {
        printf("[WARN] stack empty, can't pop\n");
        return STCK_EMPT;
    }

    do {
        if ((top = (sec_top -> next)) == NULL) {
            double ret_val = (sec_top -> value);
            free(sec_top);
            return ret_val;
        }
        sec_top = top;
    } while (top -> next);

    printf("[ERROR] pop_double_failed\n;")
    exit(EXIT_FAILURE);
    /*
    for (; top -> next; sec_top = sec_top -> next) {
        if ((top = (sec_top -> next)) == NULL) {
            double ret_val = (sec_top -> value);
            free(sec_top);
            return ret_val;
        }
    }
    */
/*}*/