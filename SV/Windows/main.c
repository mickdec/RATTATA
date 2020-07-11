#include "headers/header.h"

int main(void)
{
    PORT = malloc(5*sizeof(char));
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        PClients[i] = malloc(sizeof(PCLIENTINFO));
        PClients[i]->id = 0;
    }
    THREADNBR = 0;
    // read_config();
    while (1)
    {
        menu();
    }
    return 0;
}