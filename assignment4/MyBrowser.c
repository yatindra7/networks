#include "browser_util.h"

#define BUF_SIZE 20
#define UIO_SIZE (BUF_SIZE - 1)

#define CMD_ERR                             \
    do {                                    \
        printf("[ERROR] INVALID CMD\n");    \
        exit(EXIT_FAILURE);                 \
    } while (0)

#define URL_ERR                             \
    do {                                    \
        printf("[ERROR] INVALID URL\n");    \
        exit(EXIT_FAILURE);                 \
    } while (0)

/* FUNCTIONS */

/**
 * user IO with variable length input supported
 * @param size [out] the address to store the string size (int *)
 * @return buf the string (char *)
*/

char * uio( int *size );

/**
 * handling the request input 
 * @param cmd [in] the command (char *)
*/

void handle_request(char * cmd);

/**
 * parsing and validating a command
 * 
 * @param cmd [in] the input string (char *)
 * @param url [out] address for the url output (char **)
 * @param path [out] address for the path (char **)
 * @param req [out] address for the request (char **)
*/

void parse_cmd();

int main () {

    // TAKING THE USER INPUT IN LOOP 
    // UNTIL "QUIT" IS ENTERED

    int cmd_size;
    char *cmd;

    while ( 1 ) {

        // THE PROMPT
        printf("MyOwnBrowser> ");

        // THE INPUT
        cmd = uio( &cmd_size );

        if ( strcmp(cmd, "") == 0 ) {
            
            free(cmd);
            continue;
        }

        // HANDLE COMMAND
        if ( strcasecmp(cmd, "QUIT") == 0 )
            exit(EXIT_SUCCESS);

        handle_request(cmd);
        free(cmd);
    }

    exit(EXIT_SUCCESS);
}

char * uio( int *size ) {

    char tmp_buf[BUF_SIZE];
    int current_buf_size = 0;
    
    // malloc'ing the buffer
    char *buf;
    char *res;
    MALLOC_SAFE(char, BUF_SIZE, buf);

    current_buf_size += BUF_SIZE;
    memset(buf, 0, BUF_SIZE); // Empty string to initialize the buffer


    while ((res = fgets(tmp_buf, BUF_SIZE, stdin)) != NULL) {
        // CHECK IF BUFFER IS NOT COMPLETELY FULL

        /*
            Note this also includes the '\n'
            Example: strlen("CAFE\n") == 5; (and not 4)
            Hence the '\n' is at the index "strlen - 1"
        */
        int offset = strlen(tmp_buf);

        // Adding the strings
        strcat(buf, tmp_buf);

        // The read is done
        if (offset != UIO_SIZE) {
            int end = (((int)strlen(buf)) - 1);
            buf[end] = '\0';
            break;
        }

        // realloc for increasing the "buf" size
        current_buf_size += BUF_SIZE;
        REALLOC_SAFE(char, current_buf_size, buf);
    }

    // SETTING THE OUTPUT
    *size = strlen(buf);
    return buf;
}

void handle_request(char *cmd) {

    // WE FIRST NEED TO PARSE THE REQUEST
    // WE ALSO NEED TO CHECK IT FOR INPUT ERRORS

    /*<IMPLEMENTATION-NOTE> We define the variables
    here instead of inside the parse_cmd function
    so we can free the pointers appropriately*/

    char *url, *path, *req;         /*!< variables to store results 
                                    post parsing*/

    int port;

    parse_cmd(
                cmd
                , &url
                , &path
                , &req
                , &port
                );

    handle_connection(url, path, req, port);

    free(url); free(path); free(req);
}

/**
 * parsing utils
*/

int starts_with(const char *str, const char *start_str) {
    return strncmp(str, start_str, strlen(start_str));
}


void validate_url(char *url) {

    // THE STRING STARTS WITH http://
    if ( starts_with(url, HTTP_START) != 0 )
        URL_ERR;
    
}

void parse_cmd(
                char *cmd
                , char **url
                , char **path
                , char **req
                , int *port
                ) {

    const char *delim = " ";

    char *token = strtok(cmd, delim);

    // INITIAL TOKEN IS THE REQUEST TYPE
    // malloc'ing THE SPACE
    
    if ( strcasecmp( token, "GET" ) == 0 ) {
        *req = (char *) malloc( 4 * sizeof (char) );
        strcpy( *req, "GET" );
    }
    else if ( strcasecmp( token, "PUT" ) == 0 ) {
        *req = (char *) malloc(3 * sizeof (char) );
        strcpy( *req, "PUT");
    }
    else
        CMD_ERR;
    
    token = strtok(NULL, delim);

    // GETTING THE URL
    int len_url = strlen(token);

    validate_url(token);
    *url = (char *) malloc ( 
                            ( len_url + 1) 
                            * 
                            sizeof (char) 
                            );

    // ASSIGNING THE TOKEN TO THE URL
    strcpy( *url, token + strlen(HTTP_START));

    // FINDING OUT THE PATH
    int path_start = strchr(token + strlen(HTTP_START), '/') - token;

    // printf("path_start:%d|len_url:%d\n", path_start, len_url);
    if ( path_start == ( len_url - 1 ) ) {

        size_t size = strlen(DEFAULT);

        *path = (char *) malloc ( ( size + 1 ) * sizeof (char) );
        strcpy( *path, DEFAULT );
    }
    else {
        // malloc'ing THE SPACE
        *path = (char *) malloc ( 
                                (len_url - path_start + 1) 
                                *
                                sizeof (char) 
                                );

        // ASSIGNING TO PATH AND MAKING URL JUST THE URL
        strcpy( *path, token + path_start );
    }

    (*url)[path_start - strlen(HTTP_START)] = '\0';

    // getting the port
    char *port_str;
    if ( ( port_str = strrchr(*url, ':') ) == NULL )
        *port = 80;
    else {
        *port = atoi(port_str + 1);

        // making the URL into just the host
        port_str[0] = '\0';
    }


    return;

}