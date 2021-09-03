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
    char *port;
};

/**
 * @brief Set up the client.
 * 
 */
void client_init(struct client *client, const char *name);

void client_start(struct client *client);

void client_end(SOCKET _socket);

void client_recv_from_serv(struct client *client);

void client_handle_input(struct client *client, fd_set *reads);

void client_connected(SOCKET _socket, const char *username);

static void client_setup_windows(void);

#endif //CLIENT_H
