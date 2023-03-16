// Global Thread State
#include <pthread.h>
#include <poll.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "mysocket.h"

// the message type

#define MSG_LEN 5000

typedef struct _message {
    char content[MSG_LEN];
    int msg_len;
    int flags;
} message;

typedef int msg_size_t;

#define TRUE 1
#define FALSE 0
#define LIMIT_LEN 10
#define _INVALID_SOCKET -1
#define _CHUNK_SIZE 1000
#define NUM_SOCKS 1
#define RECV_LEN_SIZE sizeof(msg_size_t)
#define __HANDLE_ERR_IN_UTILS 1

#define SEND_ERR_MSG "[ERROR] send failed"
#define RECV_ERR_MSG "[ERROR] recv failed"

// THREAD CREATIPON UTILS
#define PTHREAD_CREATE_ERR                      \
    do {                                        \
        printf("[ERROR] pthread create failed");\
        exit(EXIT_FAILURE);                     \
    } while (0)

#define PTHREAD_SETYPE_ERR                      \
    do {                                        \
        printf("[ERROR] pthread setype failed");\
        exit(EXIT_FAILURE);                     \
    } while (0)

#define PTHREAD_CREATE(THREAD,ATTRS,FUNC,ARGS)                          \
    do {                                                                \
        int __ret = pthread_create(&THREAD, ATTRS, FUNC, ARGS);         \
        if ( __ret != 0 ) PTHREAD_CREATE_ERR;                           \
        int otp;                                                        \
        __ret = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &otp);   \
        if ( __ret != 0 ) PTHREAD_SETYPE_ERR;                           \
    } while (0)

// COND VARIABLE CREATIPON UTILS
#define PTHREAD_COND_INIT_ERR                           \
    do {                                                \
        printf("[ERROR] pthread_cond_init failed");     \
        exit(EXIT_FAILURE);                             \
    } while (0)

#define PTHREAD_COND_INIT(CV)                                   \
    do {                                                        \
        int __ret = pthread_cond_init(&CV,NULL);                \
        if ( __ret != 0 ) PTHREAD_COND_INIT_ERR;                \
    } while (0)

// MUTEX CREATIPON UTILS
#define PTHREAD_MUTEX_INIT_ERR                          \
    do {                                                \
        printf("[ERROR] pthread_mutex_init failed");    \
        exit(EXIT_FAILURE);                             \
    } while (0)

#define PTHREAD_MUTEX_INIT(LC)                                  \
    do {                                                        \
        int __ret = pthread_mutex_init(&LC,NULL);               \
        if ( __ret != 0 ) PTHREAD_MUTEX_INIT_ERR;               \
    } while (0)

// MUTEX LOCK MACROS
#define PTHREAD_MUTEX_LOCK_ERR                          \
    do {                                                \
        printf("[ERROR] pthread_mutex_lock failed");    \
        exit(EXIT_FAILURE);                             \
    } while (0)

#define PTHREAD_MUTEX_LOCK(LC)              \
    if ( pthread_mutex_lock(&LC) )          \
        PTHREAD_MUTEX_LOCK_ERR;

// MUTEX UNLOCK MACROS
#define PTHREAD_MUTEX_UNLOCK_ERR                        \
    do {                                                \
        printf("[ERROR] pthread_mutex_unlock failed");  \
        exit(EXIT_FAILURE);                             \
    } while (0)

#define PTHREAD_MUTEX_UNLOCK(LC)              \
    if ( pthread_mutex_unlock(&LC) )          \
        PTHREAD_MUTEX_LOCK_ERR;

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

static pthread_t R, S; /*<! The recv, and send management 
                    threads, only acessible here*/

// global state
static int sock_fd = _INVALID_SOCKET;
static int clifd = _INVALID_SOCKET;
static pthread_mutex_t recv_buffer_lock, send_buffer_lock;         /*!< the mutex variables*/
static pthread_cond_t recv_buffer_cv, send_buffer_cv;              /*!< the condition variables */
static sigset_t f_sig_set;

// init Recv Array
static message Received_Message[LIMIT_LEN];
// init Recv Array
static message Sent_Message[LIMIT_LEN];

// the send recv bytes counter
static int __recv_cnt, __send_cnt;


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

int __persistent_send(int sock_fd, void *msg, size_t msg_size
                                , int *send_ret, int flags) {

    int __send_ret;                                   /*!< holds the return value
                                                         of send for error check*/
    int bytes_sent = 0;                               /*!< [out] bytes sent */

    // send'ing in a loop
    while ( ( __send_ret = send(
                                sock_fd
                                , msg
                                , msg_size - bytes_sent
                                , flags
                                ) ) > 0 )
        bytes_sent += __send_ret;                       /* incrementing bytes_recvd*/

    // Returning
    if ( __send_ret < 0 && __HANDLE_ERR_IN_UTILS ) {
        perror(SEND_ERR_MSG);                           /* Error handling */
        exit(EXIT_FAILURE);
    }
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
 * @param recv_ret [out] (int*), address for return of the last recv call
 * @return bytes_recvd (int) number of bytes finally recv'd
 * 
*/

int __persistent_recv(int sock_fd, void *recv_buffer, size_t buffer_size
                    , int *recv_ret, int flags) {

    int bytes_recvd = 0;                            /*!< [out] number of bytes recvd*/
    int __recv_ret;                                 /*!< keeps track of recv return 
                                                        values for error handling*/

    // recv'ing in loop
    while ( ( __recv_ret = recv(sock_fd, recv_buffer + bytes_recvd /*address to write to changes*/ 
                                , buffer_size - bytes_recvd /*bytes left in byffer*/, flags) ) > 0 ) {
    
        bytes_recvd += __recv_ret;                  /* incrementing bytes_recvd*/
        
        // Checking the break condition
        if ( bytes_recvd == buffer_size ) break;
    }

    // Returning
    if ( __recv_ret < 0 && __HANDLE_ERR_IN_UTILS ) {
        perror(RECV_ERR_MSG);                       /* Error handling*/
        exit(EXIT_FAILURE);
    }
    *recv_ret = __recv_ret;                         /* setting up recv_ret*/
    return bytes_recvd;
}

int __chunk_send(int sockfd, void *buf, size_t len, int flags) {

    int num_iter = len / _CHUNK_SIZE;
    int _bytes_sent = 0, __send_ret;
    for (int itr = 0; itr < num_iter; itr ++) {

        _bytes_sent += __persistent_send(
                                        sockfd
                                        , buf + _bytes_sent
                                        , _CHUNK_SIZE
                                        , &__send_ret
                                        , flags
                                        );
    }
    return _bytes_sent += __persistent_send(
                                            sockfd
                                            , buf + _bytes_sent
                                            , (len % _CHUNK_SIZE)
                                            , &__send_ret
                                            , flags
                                            );
}

int __chunk_recv(int sockfd, void *buf, size_t len, int flags) {

    int num_iter = len / _CHUNK_SIZE;
    int _bytes_recvd = 0, __recv_ret;
    for (int itr = 0; itr < num_iter; itr ++) {

        _bytes_recvd += __persistent_recv(
                                        sockfd
                                        , buf + _bytes_recvd
                                        , _CHUNK_SIZE
                                        , &__recv_ret
                                        , flags
                                        );
    }

    return _bytes_recvd += __persistent_recv(
                                            sockfd
                                            , buf + _bytes_recvd
                                            , (len % _CHUNK_SIZE)
                                            , &__recv_ret
                                            , flags
                                            );
}

int __format_recv(int sock_fd, message *recvd_msg) {

    // try to recv the first 32 bits
    int __recv_ret;
    msg_size_t recv_size;

    __persistent_recv(
            sock_fd
            , &recv_size
            , RECV_LEN_SIZE
            , &__recv_ret
            , 0
    );

    printf("[__format_recv] size recvd, recving the msg now: %d\n", recv_size);
    (recvd_msg -> msg_len) = recv_size;

    // recving the msg
    return  __chunk_recv(
            sock_fd
            , (recvd_msg -> content)
            , (recvd_msg -> msg_len)
            , 0
    );
}

//void hamd()

// The recv handler function
void *__recv_handler(void * sock_in) {
    
    // extracting the descriptor
    // NUM_SOCK_CHANGE make it an array

    int n_sig;
    sigwait(&f_sig_set, &n_sig);

    printf("[__recv_handler] clifd: %d\n", clifd);

    while ( 1 ) {

        // we only get past here if there 
        // is a recv request

        msg_size_t peek_buf;
        // printf("[__recv_handler] size: %d\n", RECV_LEN_SIZE);

        int peek_check = recv(clifd, &peek_buf, RECV_LEN_SIZE, MSG_PEEK);

        if ( peek_check < 0 ) {
            perror("[__recv_handler][recv failed]\n");
            exit(EXIT_FAILURE);
        } else if ( peek_check == 0 ) {
            printf("[__recv_handler] disconnected\n");
            return NULL;
        } else {
            // printf("[__recv_handler] peek_check: %d|%d\n", peek_check, (int) peek_buf);
        }

        message recvd_msg;

    __format_recv(clifd, &recvd_msg);
        
        printf("[__recv_handler] msg recvd: |%s|!\n", recvd_msg.content);

        PTHREAD_MUTEX_LOCK(recv_buffer_lock);

        // waiting on the my_recv call
        while ( __recv_cnt == LIMIT_LEN )
            pthread_cond_wait(&recv_buffer_cv, &recv_buffer_lock);

        Received_Message[ __recv_cnt ].msg_len = recvd_msg.msg_len;
        strncpy(
            Received_Message[ __recv_cnt ].content
            , recvd_msg.content
            , recvd_msg.msg_len
        );

        __recv_cnt ++;

        PTHREAD_MUTEX_UNLOCK(recv_buffer_lock);
        
        if (clifd == _INVALID_SOCKET) {
            printf("[ERROR][__recv_handler] clifd _INVALID_SOCKET\n");
        }

        printf("[__recv_handler] msg weritten to the table\n");

        pthread_cond_broadcast(&recv_buffer_cv);

        printf("[__recv_handler] broadcast sent, going into wait\n");
    }

    return NULL;
}


int my_recv(int sockfd
            , void *buf, size_t len, int flags) {
    
    // The recv call maker
    // __recv_flags = flags;

    // locking the __recv_cnt
    PTHREAD_MUTEX_LOCK(recv_buffer_lock);

    while ( !__recv_cnt ) {
        //printf("GOING INTO DEEP SLEEP!\n");
        pthread_cond_wait(&recv_buffer_cv, &recv_buffer_lock);
    }
    printf("[my_recv] I'm up!\n");

    __recv_cnt --;

    int final_len = (len < Received_Message[ __recv_cnt ].msg_len) ? len: Received_Message[ __recv_cnt ].msg_len;

    strncpy(
        buf
        , Received_Message[ __recv_cnt ].content
        , final_len
    );

    if ( flags & MSG_PEEK )
        __recv_cnt ++;

    PTHREAD_MUTEX_UNLOCK(recv_buffer_lock);

    return final_len;
}

int __format_send(int sock_fd, message *send_msg) {

    msg_size_t msg_size = (send_msg -> msg_len);
    int __send_flags = send_msg -> flags;
    int __send_ret;

    //printf("[__format_send] clifd: %d| msg_size: %d\n", clifd, msg_size);

    __persistent_send(
                sock_fd
                , &msg_size
                , RECV_LEN_SIZE
                , &__send_ret
                , __send_flags
    );

    //printf("[__format_send] size sent: %ld\n", send_msg -> msg_len);

    return __chunk_send(
                sock_fd
                , (send_msg -> content)
                , (send_msg -> msg_len)
                , __send_flags
    );
}

// The send handler function
void *__send_handler(void *sock_in) {

    // extracting the descriptor

    int n_sig;
    sigwait(&f_sig_set, &n_sig);

    printf("[__send_handler] clifd: %d\n", clifd);
    
    while ( 1 ) {

        message send_msg;
        PTHREAD_MUTEX_LOCK(send_buffer_lock);

        // waiting on the my_send call
        while ( !__send_cnt )
            pthread_cond_wait(&send_buffer_cv, &send_buffer_lock);

        //printf("[__send_handler] done !\n");

        __send_cnt --;
        send_msg.msg_len = Sent_Message[ __send_cnt ].msg_len;
        strncpy(
            send_msg.content
            , Sent_Message[ __send_cnt ].content
            , send_msg.msg_len
            );
        send_msg.flags = Sent_Message[ __send_cnt ].flags;

        pthread_cond_broadcast(&send_buffer_cv);
        PTHREAD_MUTEX_UNLOCK(send_buffer_lock);

        //printf("[__send_handler] waking up main thread!\n");

        printf("[__send_handler] calling __format_send: %s\n", send_msg.content);

        if (clifd == _INVALID_SOCKET) {
            printf("[ERROR][__send_handler] clifd _INVALID_SOCKET\n");
        }
        __format_send(clifd, &send_msg);

        printf("[__send_handler] done with __format_send: %s sent\n", send_msg.content);
    }

    return NULL;
}

int my_send(int sockfd
            , const void *buf, size_t len, int flags) {
    
    // The send call

    //printf("[my_send] start\n");

    // locking the send queue
    PTHREAD_MUTEX_LOCK(send_buffer_lock);

    while ( __send_cnt == LIMIT_LEN ) {
        printf("[my_send] the send buf is full\n");
        pthread_cond_wait(&send_buffer_cv, &send_buffer_lock);
        printf("[my_send] recvd a signal!\n");
    }

    //printf("[my_send] Lock Acquired\n");

    Sent_Message[ __send_cnt ].msg_len = len;
    strncpy(
        Sent_Message[ __send_cnt ].content
        , buf
        , len + 1
        );
    Sent_Message[ __send_cnt ].flags = flags;

    // setting the shared variables
    __send_cnt ++;

    printf("[my_send] appended to Send_Messages:%s\n", Sent_Message[ __send_cnt - 1 ].content);

    pthread_cond_broadcast(&send_buffer_cv);

    // pthread_cond_wait(&send_buffer_cv, &send_buffer_lock);
    PTHREAD_MUTEX_UNLOCK(send_buffer_lock);
    // signalling the S thread

    //printf("[my_send] Broadcast done, woken up S!\n");

    return len;
}

int my_socket(int domain, int type, int protocol) {
    
    // socket creation
    SOCKET(sock_fd);

    sigemptyset(&f_sig_set);
    sigaddset(&f_sig_set, SIGUSR1);
    sigaddset(&f_sig_set, SIGSEGV);

    // init the pthread variables
    // if the socket is successfully
    // created
    PTHREAD_COND_INIT(recv_buffer_cv);
    PTHREAD_COND_INIT(send_buffer_cv);

    PTHREAD_MUTEX_INIT(recv_buffer_lock);
    PTHREAD_MUTEX_INIT(send_buffer_lock);

    pthread_sigmask(SIG_BLOCK, &f_sig_set, NULL);

    // thread creation
    PTHREAD_CREATE(R, NULL, __recv_handler, NULL);
    PTHREAD_CREATE(S, NULL, __send_handler, NULL);

    return sock_fd;
}

int my_bind(int sockfd
            , const struct sockaddr *addr, socklen_t addrlen) {

    // doing a bind call, and no error checking

    printf("[my_bind] start\n");

    return bind(
        sockfd
        , addr
        , addrlen
    );
}

int my_listen(int sockfd, int backlog) {

    // doing a listen on the socket

    printf("[my_listen] start\n");

    return listen(
        sockfd
        , backlog
    );
}

int my_accept(int sockfd
                , struct sockaddr *addr, socklen_t *addrlen) {

    // doing an accept call on the socket

    printf("[my_accept] start\n");

    clifd = accept(
        sockfd
        , addr
        , addrlen
    );
    
    pthread_kill(R, SIGUSR1);
    printf("[my_accept] kill R\n");
    pthread_kill(S, SIGUSR1);
    printf("[my_accept] kill S\n");

    return clifd;
}

int my_connect(int sockfd
                , struct sockaddr *addr, socklen_t addrlen) {

    // doing a connect call on the server

    printf("[my_connect] start\n");

    clifd = sock_fd;
    int ret = connect(
        sockfd
        , addr
        , addrlen
    );

    pthread_kill(R, SIGUSR1);
    pthread_kill(S, SIGUSR1);

    return ret;
}

void my_close(int sockfd) {

    printf("[my_close] start\n");

    if (sock_fd == sockfd) {
        printf("[my_close] starting cancelling\n");

        sleep(5);

        pthread_cancel(S);
        printf("[my_close] S cancelled\n");
        pthread_cancel(R);
        printf("[my_close] R cancelled\n");
    }

    close(sock_fd);

    printf("[my_close] done closing/not sock_fd\n");
    printf("returning :)\n");
}