#ifndef CLIENT_H
#define CLIENT_H

#include "socket.h"
#include "utils.h"
#include "server.h"

struct client
{
    char *name;
    struct addrinfo *peer_address; /* Allocated con malloc. Must be freed. */
    SOCKET peer_socket;
};

/**
 * @brief Set up the client.
 * 
 */
void client_init(struct client *client, const char *name, const char *port);

void client_start(struct client *client);

void client_end(SOCKET _socket);

#endif //CLIENT_H
