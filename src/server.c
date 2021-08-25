#include "server.h"

int server_start(void)
{
    SET_YELLOW_FG;
    printf("Starting server...\n"
           "Configuring socket...\n");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);

    printf("Creating socket...\n");
    SOCKET socket_listen = socket(bind_address->ai_family,
                                  bind_address->ai_socktype,
                                  bind_address->ai_protocol);
    server_check_socket(socket_listen);

    printf("Binding socket...\n");
    server_check_bind(bind(socket_listen, bind_address->ai_addr,
                           bind_address->ai_addrlen));
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    server_listen(socket_listen, 10);

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connection...\n");

    while (true)
    {
        // Save a copy of master.
        fd_set reads = master;
        server_check_select(select(max_socket + 1, &reads, 0, 0, 0));

        for (SOCKET i = 1; i <= max_socket + 1; i++)
        {
            if (FD_ISSET(i, &reads))
            {
                if (i == socket_listen)
                    /* Print info of the new connection. */
                    server_new_connection(socket_listen, &master, &max_socket);
                else
                    server_handle_client_msg(max_socket, &master, socket_listen, i);
            }
        }
    }

    return 0;
}

void server_new_connection(SOCKET socket_listen, fd_set *master, SOCKET *max_socket)
{
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);

    SOCKET socket_client = server_accept(socket_listen,
                                         (struct sockaddr *)&client_address,
                                         &client_len);

    /*
     * Add the new socket to master. And if it's necessary 
     * update max sockets.
     */
    FD_SET(socket_client, master);
    if (socket_client > *max_socket)
        *max_socket = socket_client;

    char host_buffer[1024];
    struct sockaddr *address_info = (struct sockaddr *)&client_address;

    getnameinfo(address_info, client_len, host_buffer,
                sizeof(host_buffer), 0, 0, NI_NUMERICHOST);

    printf("New connection from: Host: %s\n", host_buffer);
}

SOCKET server_accept(SOCKET socket_listen, struct sockaddr *client_address, socklen_t *client_len)
{
    SOCKET socket_client;
    socket_client = accept(socket_listen, client_address, client_len);
    server_check_socket(socket_client);

    return socket_client;
}

void server_handle_client_msg(SOCKET max_socket, fd_set *master,
                              SOCKET socket_listen, SOCKET current)
{
    struct message read;
    int bytes_received = recv(current, &read, sizeof(read), 0);
    server_check_recv(bytes_received);

    if (read.type == CONNECT)
        printf("New user: " GREEN_FG "%s\n" YELLOW_FG, read.username);

    /* Here start chatting. */
    for (SOCKET j = 1; j <= max_socket; ++j)
    {
        if (FD_ISSET(j, master))
        {
            if (j == socket_listen || j == current)
                continue;
            else
                socket_send(j, &read, sizeof(read), 0);
        }
    }
}

int main(int argc, char const *argv[])
{
    server_start();
    return 0;
}
