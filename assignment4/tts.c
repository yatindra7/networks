#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRCAT_ERR                       \
    do                                   \
    {                                    \
        perror("[ERROR] strcat failed"); \
        exit(EXIT_FAILURE);              \
    } while (0)

#define MALLOC_ERR                        \
    do                                    \
    {                                     \
        perror("[ERROR] MALLOC FAILED "); \
        exit(EXIT_FAILURE);               \
    } while (0)

#define REALLOC_ERR                        \
    do                                     \
    {                                      \
        perror("[ERROR] REALLOC FAILED "); \
        exit(EXIT_FAILURE);                \
    } while (0)

// malloc MACRO
#define MALLOC_SAFE(TYPE, NUM, PTR)                        \
    do                                                     \
    {                                                      \
        TYPE *_tmp = (TYPE *)malloc((NUM) * sizeof(TYPE)); \
        if (_tmp == NULL)                                  \
            MALLOC_ERR;                                    \
        else                                               \
            PTR = _tmp;                                    \
    } while (0)

// realloc MACRO
#define REALLOC_SAFE(TYPE, NUM, INIT_PTR)                     \
    do                                                        \
    {                                                         \
        void *_tmp = realloc(INIT_PTR, (NUM) * sizeof(TYPE)); \
        if (_tmp == NULL)                                     \
            REALLOC_ERR;                                      \
        else                                                  \
            INIT_PTR = _tmp;                                  \
    } while (0)

char *http_request = NULL;

char *append_to_http_request(char *header) {

    // reallocate the request, and append the header

    size_t curr;

    if ( http_request != NULL )
        curr = strlen(http_request);
    else
        curr = 0;
    size_t size_header = strlen(header);

    REALLOC_SAFE( char, ( curr + size_header + 1 ), http_request );

    // concatenating the header
    if ( strcat( http_request, header ) == NULL )
        STRCAT_ERR;
    
    return http_request;
}

int main () {
    append_to_http_request("test 1");
    printf("STR{%s}LEN{%ld}\n", http_request, strlen(http_request));
    append_to_http_request("test 2");
    printf("STR{%s}LEN{%ld}\n", http_request, strlen(http_request));
    append_to_http_request("test 3");
    printf("STR{%s}LEN{%ld}\n", http_request, strlen(http_request));
    append_to_http_request("test 4");
    printf("STR{%s}LEN{%ld}\n", http_request, strlen(http_request));
    append_to_http_request("test 5");
    printf("STR{%s}LEN{%ld}\n", http_request, strlen(http_request));
    append_to_http_request("test 6");
    printf("STR{%s}LEN{%ld}\n", http_request, strlen(http_request));
}