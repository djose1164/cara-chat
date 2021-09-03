#include "chat.h"
#include <stdio.h>
#include <string.h>

void chat_start(void)
{
    SET_RED_FG;
    char buffer[100];
    char username[100];
    printf(
        "\t\t\aWelcome to InmortalChat.\n"
        "\tCreated and maintained by -Jose S. Daniel (@djose1164)-.\n\n"
        "Enter your nickname (use _ insted of spaces): ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    struct client client;
    memset(&client, 0, sizeof(client));
    client.name = username;

    utils_clear_terminal();
    SET_YELLOW_FG;
    printf("Opening client...\n");
    sleep(1);

    utils_clear_terminal();
    SET_RED_FG;
    printf("\aWelcome %s! :D\n", username);
    // printf("Pls, paste here the link that I've sent you: ");

    // memset(buffer, 0, sizeof(buffer));
    // fgets(buffer, sizeof(buffer), stdin);
    // buffer[strcspn(buffer, "\n")] = 0;

    // SET_YELLOW_FG;
    // printf("Validating link...\n");
    // client.port = (char *)chat_check_link(buffer);(buffer);
    // puts("Setting client's port...");
    // printf("-- port: %s\n", client.port);
    // sleep(1);

    client_init(&client, buffer);
    client_start(&client);
}

const char *chat_check_link(const char *link)
{
    bool tcp = false;
    bool ngrok = false;
    char read[256];
    strncpy(read, link, sizeof(read));
    char *token;
    char *port;

    for (token = strtok(read, ":."); token; token = strtok(NULL, ":."))
    {
        if (!strcmp("tcp", token))
            tcp = true;
        if (!strcasecmp("ngrok", token))
            ngrok = true;
        if (isdigit(token[0]))
            port = strdup(token);
    }

    printf("---- (expected port) token: %s\n", port);

    if (tcp && ngrok)
    {
        printf("Link valid!\n"
               "Continuing...\n");
        return port;
    }

    printf("Link no valid!\n");
    printf("Closing program...\nBye.\n");
    exit(-1);
}

int main(int argc, char const *argv[])
{
    chat_start();
    return 0;
}
