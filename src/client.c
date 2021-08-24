#include "client.h"

void client_init(struct client *client, const char *name, const char *port)
{
    SET_YELLOW_FOREGROUND;

    printf("Configuring client...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;

    puts("---- Setting remote address...");
    if (getaddrinfo(name, port ? port : "80", &hints, &peer_address))
        die("getaddrinfo() failed", GET_SOCKET_ERRNO());
    puts("---- Setting remote address... Done!");

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
    SET_YELLOW_FOREGROUND;
    puts("Starting connection...\n");
    if (connect(client->peer_socket, client->peer_address->ai_addr,
                client->peer_address->ai_addrlen) == -1)
        die("connect() failed", GET_SOCKET_ERRNO());
    freeaddrinfo(client->peer_address);
    puts("\t\t\a-> Connected <-\n");

    SET_RED_FOREGROUND;
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

        if (FD_ISSET(client->peer_socket, &reads))
        {
            char read[4096];
            int bytes_received = recv(client->peer_socket, read, sizeof(read), 0);
            if (bytes_received < 1)
            {
                puts("Connection closed by server.\n");
                break;
            }

            printf("Received: %s\n",read);
        }

        // ----------------
        // Sending message.
        // ----------------
#ifdef __linux__
        if (FD_ISSET(fileno(stdin), &reads))
        {
#else
        if (_kbhit())
        {
#endif //__linux__
            char read[4096];
            if (!fgets(read, sizeof(read), stdin))
                break;
            read[strcspn(read, "\n")] = 0;
            socket_send(client->peer_socket, read, strlen(read), 0);
            puts("Sent!");
            printf("%s", "-> ");
            fflush(stdout);
        }
    }
}

void client_end(SOCKET _socket)
{
    SET_YELLOW_FOREGROUND;
    puts("Closing program...\n"
         "---- Closing socket...\n");
    CLOSE_SOCKET(_socket);

#ifdef __WIN32
    WSACleanup();
#endif //__WIN32
    puts("\aBye!\n");
}