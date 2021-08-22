#include "server.h"

int server_start(void)
{
    SET_YELLOW_FOREGROUND;
    printf("Starting server...\n"
           "Configuring socket...\n");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo("45.230.216.250", "8080", &hints, &bind_address);

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
    server_listen(socket_listen, 20);

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connection...\n");

    while (true)
    {
        // Save a copy of master.
        fd_set reads;
        reads = master;
        server_check_select(select(max_socket + 1, &reads, 0, 0, 0));

        for (SOCKET i = 1; i <= max_socket; i++)
        {
            if (FD_ISSET(i, &reads))
            {
                if (i == socket_listen)
                {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client;
                    socket_client = accept(socket_listen, (struct sockaddr *)&client_address, &client_len);
                    server_check_socket(socket_client);

                    /*
                     * Add the new socket to master. And if it's necessary 
                     * update max sockets.
                     */
                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    /* Print info of the new connection. */
                    char host_buffer[1024], serv_buffer[1024];
                    struct sockaddr *address_info = (struct sockaddr *)&client_address;
                    getnameinfo(address_info, client_len, host_buffer,
                                sizeof(host_buffer), serv_buffer,
                                sizeof(serv_buffer), NI_NUMERICHOST);
                    printf("New connection from: Host:%s Serv:%s\n", host_buffer,
                           serv_buffer);
                }
                else
                {
                    char read[1024];
                    int bytes_received = recv(i, read, sizeof(read), 0);
                    server_check_recv(bytes_received);

                    /* Here start chatting. */
                    for (SOCKET j = 1; j <= max_socket; ++j)
                    {
                        if (j == socket_listen || j == i)
                            continue;
                        else
                            server_send(j, read, sizeof(read), 0);
                    }
                }
            }
        }
    }

    return 0;
}

void server_send(const int _socket, const char *buffer, const size_t len, int flag)
{
    int begin = 0;
    while (begin < len)
    {
        int sent = send(_socket, buffer + begin, len - begin, flag);
        server_check_send(begin);
        begin += sent;
    }
}