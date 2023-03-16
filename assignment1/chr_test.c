#include <stdio.h>
#include <stdlib.h>

#define STCK_EMPT -1

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

int main () {
    stack_char *stck = create_stack_char();
    push_char(&stck, 'a');
    push_char(&stck, 'b');
    push_char(&stck, 'c');
    push_char(&stck, 'd');
    push_char(&stck, 'e');   
    printf("[TOP] %c\n", top_char(&stck));
    printf("[POP] %c\n", pop_char(&stck));
    printf("[TOP] %c\n", top_char(&stck));
    printf("[POP] %c\n", pop_char(&stck));
    printf("[TOP] %c\n", top_char(&stck));
    printf("PTOP] %c\n", pop_char(&stck));
    printf("[TOP] %c\n", top_char(&stck));
    printf("[TOP] %c\n", top_char(&stck));
    printf("[POP] %c\n", pop_char(&stck));
    printf("[TOP] %c\n", top_char(&stck));
    printf("[POP] %c\n", pop_char(&stck));
    printf("[TOP] %c\n", top_char(&stck));
    printf("PTOP] %c\n", pop_char(&stck));
    printf("[TOP] %c\n", top_char(&stck));       
}