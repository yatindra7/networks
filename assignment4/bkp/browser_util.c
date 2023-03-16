#include "browser_util.h"

// GLOBAL VARIABLE TO MAINTAIN THE HTTP REQUEST
char *http_request = NULL;
char *http_response = NULL, *http_resbk = NULL;

// THE PATH, AND REQ
char *host, *path, *req, *file_pt;

// STATUS MESSAGE
char *status_message;

// IP
struct in_addr ip;

// DATETIME
char datetime[DATE_LEN], date_if_mod[DATE_LEN];

// VARIBALE FOR FILETYPE
int file_type;

// STATUS CODE IN RESPONSE
int status_code;

// VARIBALE FOR THE PORT OF THE FILE
int port;

// THE SIZE OF THE HTTP 
size_t size_http_req, content_length;
size_t recvd_len;

/**
 * TCP send function alternative that doesn't return till the data is sent
 * 
 * "persistent_send" deals with the TCP send's shortcoming of not always 
 * sending the whole data at once. This function persistently makes "send"
 * calls till all the data is sent.
 * 
 * The last return value of the send calls is returned via a pointer param,
 * but the error message display is handled already.
 * 
 * eg: "send" can send 10 bytes as "4, 6", or "2, 3, 5" or anything else,
 * persistent send will repeatedly make send calls, while keeping track of 
 * the bytes sent.
 * 
 * \example persistent_send.c
 * 
 * @param sock_fd [in] the descriptor (int) for TCP socket
 * @param msg [in] the buffer (char []) to be sent
 * @param msg_size [in] the number of bytes (size_t) to be sent
 * @param send_ret [out] (int*), address for return of the last send call
 * @return bytes_sent (int), number of bytes finally sent
 * 
*/

int persistent_send(int sock_fd, char *msg, size_t msg_size, int *send_ret) {

    int __send_ret;                                   /*!< holds the return value
                                                         of send for error check*/
    int bytes_sent = 0;                               /*!< [out] bytes sent */

    // send'ing in a loop
    while ( ( __send_ret = send(
                                sock_fd
                                , msg + bytes_sent
                                , msg_size - bytes_sent
                                , MSG_NOSIGNAL
                                ) ) > 0 ) {
        bytes_sent += __send_ret;                       /* incrementing bytes_recvd*/
                                }


    // Returning
    if ( __send_ret < 0 && __HANDLE_ERR_IN_UTILS )
        perror(SEND_ERR_MSG);                           /* Error handling */
    *send_ret = __send_ret;                             /* setting up send_ret*/
    return bytes_sent;
}

/**
 * TCP recv function alternative that only returns if either the last recv'd byte
 * is the "end" byte, or if the buffer is full
 * 
 * "presistent_recv" deals with TCP recv returning instantaneously returning with 
 * data available, hence possibly recv'ing the data in chunks.
 * 
 * eg: sender sends 10 bytes, recv might need to called twice and can return with
 * "4, 6", or "2, 3, 5" or any other combination.
 * 
 * "persistent_recv" akes repeated recv calls with a given buffer size, until
 * either the buffer fills up, or a delimiter is reached.
 * 
 * \example persistent_recv.c
 * 
 * @param sock_fd [in] the descriptor (int) for TCP soxket
 * @param recv_buffer [out] address of the buffer (char*) to read into
 * @param buffer_size [in] the number of bytes that can be recv'd
 * @param end [in] the character (char) signifying the stream end 
 * @param recv_ret [out] (int*), address for return of the last recv call
 * @return bytes_recvd (int) number of bytes finally recv'd
 * 
*/

int persistent_recv(int sock_fd, char *recv_buffer, size_t buffer_size
                    , char end, int *recv_ret) {

    int bytes_recvd = 0;                            /*!< [out] number of bytes recvd*/
    int __recv_ret;                                 /*!< keeps track of recv return 
                                                        values for error handling*/

    // recv'ing in loop
    while ( ( __recv_ret = recv(sock_fd, recv_buffer + bytes_recvd /*address to write to changes*/ 
                                , buffer_size - bytes_recvd /*bytes left in byffer*/, 0) ) > 0 ) {
    
        bytes_recvd += __recv_ret;                  /* incrementing bytes_recvd*/
        
        // Checking the break condition
        if ( recv_buffer[bytes_recvd - 1] == end ) break;
        if ( bytes_recvd == buffer_size ) break;
    }

    // Returning
    if ( __recv_ret < 0 && __HANDLE_ERR_IN_UTILS )
        perror(RECV_ERR_MSG);                       /* Error handling*/
    *recv_ret = __recv_ret;                         /* setting up recv_ret*/
    return bytes_recvd;
}


int create_socket_and_connect_to_server() {

    int sock_fd;                            /*!< the client socket descriptor*/
    struct sockaddr_in server_addr;         /*!< struct for server address */

    SOCKET(sock_fd);
    
    // getting the IP
    struct hostent *bhost = gethostbyname(host);
    ip = * ( (struct in_addr *) ( bhost -> h_addr ) );

    // printf("[create_socket_and_connect_to_server] IP:%s\n", inet_ntoa(ip));

    SERVER_ADDR_INIT(server_addr, ip, port);

    CONNECT(sock_fd, server_addr);

    printf("SOCK_FD: %d\n", sock_fd);

    return sock_fd;
}

int get_file_type() {

    // getting the file type from the extension

    char *ext = strrchr( path, '.' );

    if ( ext == NULL )
        return HTML;
    if ( strcmp( path, "/" ) == 0 )
        return HTML;
    if ( strcmp( ext, ".jpg" ) == 0 )
        return IMG;
    if ( strcmp( ext, ".html" ) == 0 )
        return HTML;
    if ( strcmp( ext, ".pdf" ) == 0 )
        return PDF;
    
    // if neither matches
    return MISC;
}

// utility to concatenate headers one over the other
char *append_to_http_request(char *header) {

    // reallocate the request, and append the header

    size_t curr;

    if ( http_request != NULL )
        curr = strlen(http_request); //size_http_req;
    else {
        curr = size_http_req = 0;
        http_request = "\0";
    }

    if ( size_http_req != strlen(http_request) ) {
        printf("BRUH: %ld|%ld\n", size_http_req, strlen(http_request));
    }

    size_t size_header = strlen(header);

    REALLOC_SAFE( char, ( curr + size_header + 1 ), http_request );

    size_http_req += size_header;

    if (curr == 0) 
        strcpy(http_request, "");

    // concatenating the header
    if ( strcat( http_request, header ) == NULL )
        STRCAT_ERR;

    return http_request;
}

void set_date_if_mod(struct tm *date) {

    // set time
    const time_t day = 24 * 60 * 60;
    time_t datesecs = mktime(date) - 2 * day;

    // final time
    struct tm date_mod = *localtime(&datesecs);

    strftime(
            date_if_mod
            , sizeof date_if_mod
            , "%a, %d %b %Y %H:%M:%S GMT"
            , &date_mod
            );
}

void set_date() {

    memset(datetime, 0, DATE_LEN);
    memset(date_if_mod, 0, DATE_LEN);
    
    // creating the string
    time_t now = time(0);
    struct tm tm = *gmtime( &now );

    strftime(
            datetime
            , sizeof datetime
            , "%a, %d %b %Y %H:%M:%S %Z"
            , &tm
            );
    set_date_if_mod(&tm);
}

void append_for_img () {

    // Accept header
    append_to_http_request("Accept: image/jpeg");
    APPND_NWLINE;
}

void append_for_html () {

    // Accept header
    append_to_http_request("Accept: text/html");
    APPND_NWLINE;
}

void append_for_pdf () {

    // Accept header
    append_to_http_request("Accept: application/pdf");
    APPND_NWLINE;
}

void append_for_misc () {

    // Accept header
    append_to_http_request("Accept: text/*");
    APPND_NWLINE;
}

void append_first_line() {
    // adding first line
    // append_to_http_request(req);
    
    //append_to_http_request(req);
    MALLOC_SAFE(char, 4, http_request);
    strcpy( http_request, req );
    size_http_req += 3;

    APPND_SPACE;

    append_to_http_request(path);

    if ( strcmp(req, "PUT") == 0 ) {

        int len_pth = strlen(path);

        if ( path[len_pth - 1] != '/' )
            append_to_http_request("/");
        append_to_http_request(file_pt);
    }

    //printf("BRUH\n");

    APPND_SPACE;

    append_to_http_request(HTTP);

    APPND_NWLINE;
}

void append_host_header() {
    // appending Host Header
    append_to_http_request("Host: ");

    append_to_http_request(host);

    APPND_NWLINE;
}

void append_connection_header() {
    // appending the Connection Header
    append_to_http_request("Connection: close");

    APPND_NWLINE;
}

void append_date_header() {
    // appending the Date Header
    append_to_http_request("Date: ");

    set_date();
    append_to_http_request(datetime);

    APPND_NWLINE;
}

void append_accept_language() {
    // appending Accept Language Header
    append_to_http_request("Accept-Language: en-us, en;q=0.7");

    APPND_NWLINE;
}

void append_if_modified_since() {
    // appending If-Modified-Since
    append_to_http_request("If-Modified-Since: ");

    append_to_http_request(date_if_mod);

    APPND_NWLINE;
}

void append_content_type() {
    
    char *ext = strrchr(file_pt, '.');

    printf("START_CTYPE\n");

    append_to_http_request("Content-Type: ");
    if ( strcmp(ext, ".pdf") == 0 ) {
        append_to_http_request("application/pdf");
    } else if ( strcmp(ext, ".html") == 0 ) {
        append_to_http_request("text/html");
    } else if ( strcmp(ext, ".jpeg") == 0 ) {
        append_to_http_request("image/jpeg");
    } else {
        append_to_http_request("text/*");
    }

    APPND_NWLINE;
}

size_t get_file_len(char* path){
    FILE *file;
    unsigned long fileLen;

    // Open file
    file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Unable to open file %s", path);
        return -1;
    }

    // Get file length
    fseek(file, 0, SEEK_END);
    fileLen=ftell(file);
    fclose(file);
    return fileLen;

}

void append_content_length() {

    append_to_http_request("Content-Length: ");

    size_t con_len = get_file_len(file_pt);

    printf(">%ld\n", con_len);

    char con_len_buf[10]; memset(con_len_buf, 0, 10);

    sprintf(con_len_buf, "%ld", con_len);

    append_to_http_request(con_len_buf);

    APPND_NWLINE;
}

char* get_file_content(char* path){
    FILE *file;
    char buffer[50];
    char *res = NULL;
    size_t n;

    // Open file
    file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Unable to open file %s\n", path);
        return NULL;
    }

    // Read file and print contents
    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        res = append_to_http_request(buffer);
    }

    fclose(file);
    return res;
}

char *create_http_request() {

    // file type given
    file_type = get_file_type();

    append_first_line();

    append_host_header();

    append_connection_header();

    append_date_header();

    if ( strcmp(req, "GET") == 0 ) {
        append_accept_language();

        append_if_modified_since();

        // creating the body of the HTTP request
        // making the file dependent headers
        if ( file_type == IMG ) {
            
            // append for txt
            append_for_img();
        } else if ( file_type == HTML ) {
            
            // append for html
            append_for_html();
        } else if ( file_type == PDF ) {

            // append for pdf
            append_for_pdf();
        } else if ( file_type == MISC ) {

            // append for misc
            append_for_misc();
        } else
            FILE_ERR;

        APPND_NWLINE;
    } else if ( strcmp(req, "PUT") == 0 ) {

        // PUT request
        append_content_type();
        
        append_content_length();

        APPND_NWLINE;

        get_file_content(file_pt);

        APPND_NWLINE;
    }

    printf("SIZEOF_REQUEST: %ld\n", strlen(http_request));
    return http_request;
}

/**
 * <<<<THE PART OF THE CODE TO HANDLE THE INCOMING HTTP RESPONSE FOLLOWS>>>>
*/

/**
 * constantly recvs into a buffer, and appends to the http_response
*/


void parse_first_line() {

    if ( http_response == NULL )
        RESPON_ERR;

    char *token = strtok(http_response, " ");

    if ( strcasecmp(token, HTTP) != 0 )

        // invalid response error
        RESP_VER_ERR;
    
    token = strtok(NULL, " ");

    // getting the status code

    int len_code = strlen( token );
    status_code = atoi( token );

    // getting the msg
    token += (len_code + 1);

    char *end_of_msg = strchr(token, '\n');
    *end_of_msg = '\0';

    // status message
    int len_status_msg = strlen( token );

    MALLOC_SAFE(char, len_status_msg + 1, status_message);

    // strcpy
    strcpy( status_message, token );
}

void handle_status() {

    // status codes
    if ( status_code == 200  ||
         status_code == 400  || 
         status_code == 403  || 
         status_code == 404)
         printf("[%d:STATUS MSG] %s\n", status_code, status_message);
    else
        printf("Unknown error\n");
}

void get_content_length(/*void *headers*/) {

    char *con_len = strstr(http_response, "Content-Length");

    //printf("CON_LEN\n");

    if (con_len == NULL) {
        //printf("TODO\n");
        // content_length = recvd_len - (strstr(http_response, "\r\n\r\n") - http_response + 4) - 2;
        return;
    }

    int i = 0;
    //printf("K\n");
    for (i = 0; ; i ++) {
        printf("%c", con_len[i]);
        if ( (con_len[i] == ':') || (con_len[i] == ' ') || (con_len[i] = '\t') || isalnum(con_len[i]) )
            continue;
        else break;
    }

    char *token = strtok(con_len, " ");

    content_length = atoi(token);

    //char *token = strtok(con_len, " ");

    //printf("<<<<<%s>>>>>", token);

    // content_length = atoi(token);
}

void parse_http_response() {

    printf("PARSING START\n");
    parse_first_line();

    handle_status();
    
    //char *headers = strchr(http_resbk, '\n') + 1;
    get_content_length(/*headers*/);
}
char *append_to_http_response(char *recvd) {

    size_t curr;

    if ( http_response != NULL )
        curr = strlen(http_response);
    else {
        curr = recvd_len = 0;
    }

    //printf("http_response: %s, size: %ld", http_response, curr);

    size_t size_header = strlen(recvd);
    //printf("header_size: %ld\n", size_header);

    REALLOC_SAFE( char, ( curr + size_header + 2 ), http_response );

    if (curr == 0) 
        strcpy(http_response, "");

    // concatenating the header
    if ( strcat( http_response, recvd ) == NULL )
        STRCAT_ERR;
    
    //printf("http_response size: %ld\n\n", curr + size_header);

    return http_response;
}

char *append_to_http_resbk(char *recvd) {

    size_t curr;

    if ( http_resbk != NULL )
        curr = strlen(http_resbk);
    else {
        curr = recvd_len = 0;
    }
    size_t size_header = strlen(recvd);

    REALLOC_SAFE( char, ( curr + size_header + 2 ), http_resbk );

    recvd_len += size_header;

    if (curr == 0) 
        strcpy(http_resbk, "");

    // concatenating the header
    if ( strcat( http_resbk, recvd ) == NULL )
        STRCAT_ERR;
    
    return http_resbk;
}

int get_data_chunks (int sock_fd, int offset){

    // data to be collected
    int to_be_recvd = content_length - offset;

    char recv_buf[BUFFER_SIZE];
    memset(recv_buf, 0, BUFFER_SIZE);

    int __bytes_recvd = 0, _recv_ret;

    while ( ( _recv_ret = recv(
                                sock_fd
                                , recv_buf + __bytes_recvd
                                , to_be_recvd - __bytes_recvd
                                , 0
    ) ) > 0 ) {

        __bytes_recvd += _recv_ret;

        //append_to_http_response(recv_buf);
        append_to_http_resbk(recv_buf);
    }

    if ( _recv_ret < 0 ) return -1;                         // there is an error during the recv call
    else if ( __bytes_recvd != to_be_recvd ) return -1;     // the other side disconnected and hence recv returned 0
    else return __bytes_recvd;                              // no error
}

int recv_http_response (int sock_fd) {

    char recv_buf[BUFFER_SIZE];
    memset(recv_buf, 0, BUFFER_SIZE);

    int _recv_ret;
    size_t bytes_recvd = 0;
    //char *data_start;

    while ( ( _recv_ret = recv(
                                sock_fd
                                , recv_buf
                                , (BUFFER_SIZE - 1)
                                , 0
    )) > 0 ) {

        // incrementing
        bytes_recvd += _recv_ret;

        recv_buf[_recv_ret] = '\0';


        append_to_http_response(recv_buf);
        append_to_http_resbk(recv_buf);

        //printf("[recv_http_response] [bytes_recvd]%ld\n", bytes_recvd);
    
        memset(recv_buf, 0, BUFFER_SIZE);
    }

    if ( _recv_ret < 0 ) {
        perror(RECV_ERR_MSG);
        return -1;
    } else {
        
        //printf("[DEBUG] response:\n%s\n", http_resbk);
        printf(
            "LAST_CHARS|%d|%d|%d|%d|\n",
            http_resbk[recvd_len - 4]
            , http_resbk[recvd_len - 3]
            , http_resbk[recvd_len - 2]
            , http_resbk[recvd_len - 1]
            );
        /*data_start = strstr(http_resbk, "\r\n\r\n") + 2;*/

        /*
        // to calculate how much data we've recvd
        int data_remain = bytes_recvd                          
                            - ( data_start - recv_buf )         // basically where the \n\n start
                            - 2;                                // the \n\n takes up 2 char
        */

        // parsing the haeders
        parse_http_response();
        
        /*
        if (get_data_chunks(sock_fd, data_remain) < 0){
            return -1;
        }*/
    }

    return recvd_len;
}

char* ext_from_type(int type) {

    if ( type == HTML )
        return ".html";
    else if ( type == IMG )
        return ".jpeg";
    else if ( type == PDF )
        return ".pdf";
    else
        return ".txt";
}

int save_file() {

    // isolating the filename from the path
    char *filename = strrchr(path, '/') + 1;

    printf("filename: %s\n", filename);

    char file_path[DIR_LEN]; memset(file_path, 0, DIR_LEN);
    // const char *home;

    /*
    if ( ( home = getenv("HOME") ) == NULL )
        strcpy(file_path, getpwuid(getuid())->pw_dir);
    else
        strcpy(file_path, home);

    if ( strcat(file_path, "/Downloads/") == NULL)
        STRCAT_ERR;
    if ( strcat(file_path, filename) == NULL)
        STRCAT_ERR;
    */

    strcpy( file_path, "/tmp/tmp_1_2_3" );
    strcat( file_path, ext_from_type(file_type) );

    printf("PATH:%s\n", file_path);

    //int sptr = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 666);

    FILE *fptr = fopen(file_path, "wb");

    size_t bytes_written = 0;
    int write_ret;

    void *data_start = (void *) (strstr(http_resbk, "\r\n\r\n") + 4);
    if (content_length == 0) {
        //

        content_length = recvd_len - ( (char *) data_start - http_resbk + 4 ) - 2;
    } 
    
    
    //char *_start = strstr(http_response, )

    printf("Content_Length: %ld", content_length);
    //printf("LOOP START\n");
    while ( 
        (write_ret =
        fwrite(
            data_start + bytes_written
            , sizeof(char)
            , content_length - bytes_written
            , fptr
        )  > 0) ) {
            bytes_written += write_ret;

            printf("Bytes: %ld\n", bytes_written);
            if (bytes_written == content_length) break;
        }
    if ( write_ret <= 0 ) {
        fclose(fptr);
        perror("[ERROR]");
        return -1;
    }

    return bytes_written;
}

/**
 * breaks down the buffer 'final' into chunks of size 'buffer_size'
 * basically sends the 
*/

int send_chunks(int sock_fd, void *final, int buffer_size, int final_size) {

    // loop breaks when we've sent final_size bytes

    //printf("[send_chunks] start\n");

    //printf("final_size: %d\n", final_size);

    int __bytes_sent = 0, __send_ret, __err_chk = 0, to_be_sent;
    to_be_sent = ( ( final_size - __bytes_sent ) > buffer_size ) ? buffer_size : ( final_size - __bytes_sent );
    while (
        ( __send_ret = persistent_send(
            sock_fd
            , final + __bytes_sent
            , to_be_sent
            , &__err_chk
        ) ) > 0
    ) {
        __bytes_sent += __send_ret;

        /*
        if ( __bytes_sent == final_size )
            break;
        */

        to_be_sent = ( ( final_size - __bytes_sent ) > buffer_size ) ? buffer_size : ( final_size - __bytes_sent );

        //printf("bytes_sent: %d|to_be_sent: %d\n", __bytes_sent, to_be_sent);
    }

    if ( __send_ret < 0 )
        return -1;
    else
        return __bytes_sent;
}

int check_valid_char(char cb, char c, char ca) {
    return (c == '\r' && ca == '\n') || (c == '\n' && cb == '\r') || isalnum(c);
}

int check_valid() {

    for (int i = 0; i < strlen(http_request); i ++) {

        if (i == 0) continue;
        if (i ==  strlen(http_request) - 1) continue;
        if (check_valid_char(http_request[i - 1], http_request[i], http_request[i + 1]) == 0)
            return i;
    }

    return -1;
}

void handle_connection(char *_host, char *_path, char *_req, char *_file_pt, int _port) {

    // setting the global variable
    host = _host;
    path = _path;
    req = _req;
    port = _port;
    file_pt = _file_pt;

    printf("host:%s|path:%s|port:%d\n", host, path, port);

    // creating the HTTP Request looking at the path
    http_request = create_http_request();

    //http_request = "GET / HTTP/1.1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: en-US,en;q=0.9\r\nCache-Control: max-age=0\r\nConnection: keep-alive\r\nCookie: _ga=GA1.3.1438497969.1670917915; __atssc=google%3B1; __atuvc=3%7C2; PHPSESSID=5u62vqhncdjlccnfi8e9em5gq2\r\nHost: cse.iitkgp.ac.in\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36\r\n\r\n";

    if (check_valid() > 0) {
        printf("ERR: %d, ", http_request[check_valid()]);
    }

    // connecting to the server
    int sock_fd = create_socket_and_connect_to_server();

    //printf("WHY\n");

    printf("\n%s", http_request);

    // DEBUG
    printf("STARTING SEND\n");

    //printf("{}%d\n", http_request[strlen(http_request) - 1]);

    int _send_ret = send_chunks(
                                sock_fd
                                , http_request
                                , BUFFER_SIZE
                                , strlen(http_request)
                                );

    if ( _send_ret != strlen(http_request) ) {
        printf("[403:STATUS MSG] %s\n", "Unable to connect to the server");
        goto set_null_exit;
    }

    // DEBUG
    printf("STARTING RECV\n");

    int _recv_ret = recv_http_response(sock_fd);

    // recv chunks
    if ( _recv_ret == -1 ) {
        printf("[ERROR] Message recv'ing error\n");
        goto set_null_exit;
    } else if ( _recv_ret == -2 ) {
        printf("[ERROR] Data recv'ing error\n");
        goto set_null_exit;
    }

    // DEBUG
    printf("RECV DONE\n");

    if ( strcmp(req, "GET") == 0 ) {
        // save file
        if ( save_file() < 0 )
            goto exit_handle_connection;

        // open file
        // for now
        printf("FILE SAVED\n");
    }
    // exiting connection

exit_handle_connection:
    free(http_request);
    free(http_response);
    free(http_resbk);

set_null_exit:
    http_request = http_resbk = http_response = NULL;
    recvd_len = size_http_req = 0;
    return;
}