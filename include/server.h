#ifndef SERVER_H
#define SERVER_H

#include "socket.h"
#include "utils.h"

int server_start(void);

#define server_check_recv(received)     \
    do                                  \
    {                                   \
        if ((received) < 1)             \
        {                               \
            FD_CLR(current, master);    \
            CLOSE_SOCKET(current);      \
            return;                     \
        }                               \
    } while(0)                          

static inline void server_check_socket(SOCKET _socket)
{
    if (!IS_VALID_SOCKET(_socket))
    {
        printf("socket() failed. "_ERR " %d\n", GET_SOCKET_ERRNO());
        exit(-1);
    }
}

static inline void server_check_bind(int status)
{
    if (status)
    {
        fprintf(stderr, "bind() failed. "_ERR" %d.\n", GET_SOCKET_ERRNO());
        exit(-1);
    }
}

static inline void server_listen(const unsigned socket_listen, const unsigned max)
{
    if (listen(socket_listen, max) < 0)
    {
        fprintf(stderr, "%s failed. "_ERR" %d.\n", __func__, GET_SOCKET_ERRNO());
        exit(-1);
    }
}

static inline void server_check_select(int status)
{
    if (status < 0)
    {
        fprintf(stderr, "%s failed. "_ERR" %d.\n", __func__, GET_SOCKET_ERRNO());
        exit(-1);
    }
}

void server_new_connection(SOCKET socket_listen, fd_set *master, SOCKET *max_socket);

SOCKET server_accept();

void server_handle_client_msg(SOCKET max_socket, fd_set *master,
                              SOCKET socket_listen, SOCKET current);

static inline void win_init(void)
{
#ifdef __WIN32
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        exit(-1);
    }
#endif //__WIN32
}

#endif //SERVER_H
