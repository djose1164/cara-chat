#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define SET_RED_FG printf("\033[0;31m")
#define SET_YELLOW_FG printf("\033[0;33m")
#define GREEN_FG "\x1B[32m"
#define YELLOW_FG "\033[0;33m"
#define RED_FG "\033[0;31m"
#define RESET_FG "\033[0m"
#define RESET_COLOR printf("\033[0m")

#define _ERR "Error code:"

#define die(msg, errcode)                                          \
    do                                                             \
    {                                                              \
        RESET_COLOR;                                               \
        fprintf(stderr, "%s. Error code: %d\n", (msg), (errcode)); \
        exit(EXIT_FAILURE);                                        \
    } while(0)                                                     \

#ifdef __linux__
#else
#define sleep(time) Sleep((time)*1000))
#endif // __linux__

enum message_type
{
    NORMAL,
    CONNECT,
    DISCONNECT,
    SUCCESS,
    ERROR
};

struct message
{
    enum message_type type;
    char username[50];
    char data[1024];
};


static inline void utils_clear_terminal(void)   
{
    fflush(stdout);
    system("clear||cls");
}

#endif //UTILS_H
