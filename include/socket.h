/*
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
|                   File: socket.h   ---    Public header                     |
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
|   This module contains includes and definitions the needed variables and    |
|   functions.                                                                |
|   IMPORTANT: Work upon these definitions to matain portability.             |  
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#ifndef SOCKET_H
#define SOCKET_H

#include "utils.h"

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
#define GET_SOCKET_ERRNO() WSAGetLastError()

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

static inline void socket_check_send(int status)
{
    if (status == -1)
    {
        fprintf(stderr, "%s failed. "_ERR" %d.", __func__, GET_SOCKET_ERRNO());
        exit(-1);
    }
}

static inline void socket_send(const int _socket, const char *buffer, const size_t len, int flag)
{
    int begin = 0;
    while (begin < len)
    {
        int sent = send(_socket, buffer + begin, len - begin, flag);
        socket_check_send(sent);
        begin += sent;
    }
}

#endif //SOCKET_H
