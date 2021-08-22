#ifndef SERVER_H
#define SERVER_H

#include "utils/terminal_colors.h"

#ifdef __WIN32
#ifndef __WIN32_WINNT
#define __WIN32_WINNT 0x0600
#endif //__WIN32_WINNT

#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")

#define IS_VALID_SOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSE_SOCKET(s) closesocket(s)
#define GET_SOCKET_ERRNO(s) WSAGetLastError()

#else
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCKET int

#define IS_VALID_SOCKET(s) ((s) >= 0)
#define CLOSE_SOCKET(s) close(s)
#define GET_SOCKET_ERRNO() errno
#endif // __WIN32

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define _ERR "Error code:"

#define server_check_recv(received) do {                    \
        if ((received) < 1)                                 \
        {                                                   \
            FD_CLR(i, &master);                             \
            CLOSE_SOCKET(i);                                \
            continue;                                       \
        }                                                   \
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

void server_send(const int socket, const char *buffer, const size_t len, int flag);

static inline void server_check_send(int status)
{
    if (status == -1)
    {
        fprintf(stderr, "%s failed. "_ERR" %d.", __func__, GET_SOCKET_ERRNO());
        exit(-1);
    }
}

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
