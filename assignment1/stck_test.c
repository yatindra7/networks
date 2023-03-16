#include <stdio.h>
#include <stdlib.h>

#define STCK_EMPT -1

typedef struct double_node {
    double value;
    struct double_node *next;
} stack_double;

//typedef stack_double *abs_stack_double;

int is_empty_double(stack_double *stck) {
    return (stck -> next) == NULL;
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

int main () {
    stack_double *stck = create_stack_double();
    push_double(&stck, 1.2);
    push_double(&stck, 2.3);
    push_double(&stck, 1.3);
    push_double(&stck, 2);
    push_double(&stck, 4.2);
    push_double(&stck, 5.3);
    printf("[POP] %lf\n", pop_double(&stck));
    printf("[TOP] %lf\n", top_double(&stck));
    printf("[POP] %lf\n", pop_double(&stck));
    printf("[TOP] %lf\n", top_double(&stck));
    printf("[POP] %lf\n", pop_double(&stck));
    printf("[TOP] %lf\n", top_double(&stck));
    printf("[POP] %lf\n", pop_double(&stck));
    printf("[TOP] %lf\n", top_double(&stck));
    printf("[POP] %lf\n", pop_double(&stck));
    printf("[TOP] %lf\n", top_double(&stck));
    printf("[POP] %lf\n", pop_double(&stck));
    printf("[TOP] %lf\n", top_double(&stck));
}