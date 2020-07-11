#include "headers/header.h"

int main(void)
{
    int server_initied = 0;
    THREADNBR = 0;
    PORT = malloc(5*sizeof(char));
    strcpy(PORT, DEFAULT_PORT);
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        PClients[i] = malloc(sizeof(PCLIENTINFO));
        PClients[i]->id = 0;
        PClients[i]->ip = malloc(16*sizeof(char));
    }

    read_config();

    intro();
    while (1)
    {
        menu(&server_initied);
    }

    return 0;
}