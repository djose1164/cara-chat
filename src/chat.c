#include "chat.h"
#include <stdio.h>
#include <string.h>

void chat_start(void)
{
    SET_RED_FOREGROUND;
    char buffer[100];
    printf(
        "\t\t\aWelcome to InmortalChat.\n"
        "\tCreated and maintained by -Jose S. Daniel (@djose1164)-.\n\n"
        "Enter your nickname (use _ insted of spaces): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    struct client client;
    memset(&client, 0, sizeof(client));
    client.name = buffer;

    utils_clear_terminal();
    SET_YELLOW_FOREGROUND;
    printf("Opening client...\n");
    sleep(1);

    utils_clear_terminal();
    SET_RED_FOREGROUND;
    printf("\aWelcome %s! :D\n", buffer);
    printf("Pls, paste here the link that I've sent you: ");

    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    SET_YELLOW_FOREGROUND;
    printf("Validating link...\n");
    //chat_check_link(buffer, sizeof(buffer));
    sleep(1);

    client_init(&client, buffer, "80");
    client_start(&client);
}

void chat_check_link(const char *link, size_t len)
{
    char temp[20], temp2[512];
    strncpy(temp, link, 4);
    temp[5] = '\0';
    strncpy(temp2, &link[37], 8);
    temp2[len - 7] = '\0';

    if (!strcmp("http", temp) && !strcasecmp("ngrok.io", temp2))
        printf("Link valid!\n"
               "Continuing...\n");
    else
    {
        printf("Link no valid!\n");
        printf("Closing program...\nBye.\n");
        exit(-1);
    }
}

int main(int argc, char const *argv[])
{
    chat_start();
    return 0;
}
