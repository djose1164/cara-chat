#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define SET_RED_FOREGROUND printf("\033[0;31m")
#define SET_YELLOW_FOREGROUND printf("\033[0;33m")
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

static inline void utils_clear_terminal(void)   
{
    fflush(stdout);
    system("clear||cls");
}

#endif //UTILS_H
