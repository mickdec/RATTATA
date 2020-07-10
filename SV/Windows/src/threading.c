#include "../headers/header.h"

void link_client(CLIENTINFO *Client)
{
    int iResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int id = Client->id;
    for (;;)
    {
        iResult = recv(Client->Socket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            printf("%s", recvbuf);
            memset(recvbuf, 0, 4096);
            recvbuf[0] = '\0';
            fgets(recvbuf, 4096, stdin);
            if (strcmp(recvbuf, "background") == 10)
            {
                send(Client->Socket, "", 2, 0);
                memset(recvbuf, 0, 4096);
                recvbuf[0] = '\0';
                break;
            }
            send(Client->Socket, recvbuf, strlen(recvbuf), 0);
            memset(recvbuf, 0, 4096);
            recvbuf[0] = '\0';
        }
        else
        {
            printf("Client unreachable : %d\n", WSAGetLastError());
            closesocket(Client->Socket);
            WSACleanup();

            free(PClients[id]);
            PClients[id] = malloc(sizeof(PCLIENTINFO));
            PClients[id]->id = 0;
        }
    }
}

void init_client_thread(PTHREADDATA pclientdata)
{
    hThreadArray[THREADNBR] = CreateThread(NULL, 0, init_client, pclientdata, 0, &dwThreadIdArray[THREADNBR]);

    if (hThreadArray[THREADNBR] == NULL)
    {
        printf("Error");
        ExitProcess(3);
    }

    THREADNBR++;
    printf("\n!!!!!!! %d !!!!!!!\n", THREADNBR);
}