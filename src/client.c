#include "client.h"

void client_init(struct client *client, const char *name)
{
    SET_YELLOW_FG;

    /* If windows is detected. */
    client_setup_windows();

    printf("Configuring client...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;

    puts("---- Setting remote address...");
    if (getaddrinfo("0.tcp.ngrok.io", "14212", &hints, &peer_address))
        die("getaddrinfo() failed", GET_SOCKET_ERRNO());
    puts("---- Setting remote address... Done!");
    puts("-- Freed unnecessary memory of: client->port.");

    printf("IP address is: ");
    char address_buff[1024];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buff,
                sizeof(address_buff), 0, 0, NI_NUMERICHOST);
    printf("%s\n", address_buff);

    puts("---- Creating socket...");
    SOCKET peer_socket = socket(peer_address->ai_family,
                                peer_address->ai_socktype,
                                peer_address->ai_protocol);
    if (!IS_VALID_SOCKET(peer_socket))
        die("socket() failed", GET_SOCKET_ERRNO());
    puts("---- Creating socket... Done!");

    client->peer_address = peer_address;
    client->peer_socket = peer_socket;
    printf("%s() done!\n", __func__);
}

void client_start(struct client *client)
{
    SET_YELLOW_FG;
    puts("Starting connection...\n");
    if (connect(client->peer_socket, client->peer_address->ai_addr,
                client->peer_address->ai_addrlen) == -1)
        die("connect() failed", GET_SOCKET_ERRNO());
    freeaddrinfo(client->peer_address);

    puts("\t\t\a-> Connected <-\n");
    client_connected(client->peer_socket, client->name);

    SET_RED_FG;
    printf("To send a message press enter.\n"
           "-> ");
    fflush(stdout);

    while (true)
    {
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(client->peer_socket, &reads);
#ifdef __linux__
        FD_SET(fileno(stdin), &reads);
#endif // __linux__

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        if (select(client->peer_socket + 1, &reads, 0, 0, &timeout) < 0)
            die("select() failed", GET_SOCKET_ERRNO());

        // Handle server response.
        if (FD_ISSET(client->peer_socket, &reads))
            client_recv_from_serv(client);
        // ----------------
        // Sending message.
        // ----------------
        client_handle_input(client, &reads);
    }
}

void client_recv_from_serv(struct client *client)
{
    struct message read;
    memset(&read, 0, sizeof(read));
#ifndef __linux__
    int bytes_received = recv(client->peer_socket, (char *)&read, sizeof(read), 0);
#else
    int bytes_received = recv(client->peer_socket, &read, sizeof(read), 0);
#endif // __linux__
    if (bytes_received < 1)
    {
        puts("\a\rConnection closed by server.\n");
        client_end(client->peer_socket);
    }
    
    switch (read.type)
    {
    case CONNECT:
        printf(GREEN_FG "\r%s " RED_FG "has connected.\n", read.username);
        break;

    case DISCONNECT:
        printf(GREEN_FG "\r%s " RED_FG "has disconnected.\n", read.username);
        break;

    case NORMAL:
        printf(GREEN_FG "\r\a%s" RED_FG ": %s\n", read.username, read.data);
        break;

    default:
        RESET_COLOR;
        fprintf(stderr, "\rUknown type message.\n");
        break;
    }
    printf("-> ");
}

void client_handle_input(struct client *client, fd_set *reads)
{
#ifdef __linux__
    if (FD_ISSET(fileno(stdin), reads))
    {
#else
    if (_kbhit())
    {
#endif //__linux__
        char read[1024] = "";

        struct message msg;
        memset(&msg, 0, sizeof(msg));
        msg.type = NORMAL;

        strncpy(msg.username, client->name, sizeof(client->name));
        if (!fgets(read, sizeof(read), stdin))
            return;
        read[strcspn(read, "\n")] = 0;

        strncpy(msg.data, read, sizeof(read));

        socket_send(client->peer_socket, &msg, sizeof(msg), 0);
        printf("%s", "-> ");
    }
    fflush(stdout);
}

void client_connected(SOCKET _socket, const char *username)
{
    struct message msg = {0};
    strncpy(msg.username, username, sizeof(msg.username));
    msg.type = CONNECT;
    socket_send(_socket, &msg, sizeof(msg), 0);
}

void client_end(SOCKET _socket)
{
    SET_YELLOW_FG;
    puts("---- Closing program...\n"
         "-- Closing socket...\n");
    CLOSE_SOCKET(_socket);

#ifdef __WIN32
    WSACleanup();
#endif //__WIN32
    puts("\aBye!\n");
    exit(0);
}

/* Private functions. */

static void client_setup_windows()
{
#ifdef __WIN32
    puts("---- Windows detected.\n"
         "-- Initialing...");
         
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        ("Failed to initialize.\n");
        -1;
    }

    puts("-- Initialing... Done!");
#endif // __WIN32
}