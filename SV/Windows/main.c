#include "headers/header.h"

int main(void)
{
    THREADNBR = 0;
    PORT = malloc(5*sizeof(char));
    strcpy(PORT, DEFAULT_PORT);
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        PClients[i] = malloc(sizeof(PCLIENTINFO));
        PClients[i]->id = 0;
    }
    // read_config();
    while (1)
    {
        menu();
    }
    return 0;
}