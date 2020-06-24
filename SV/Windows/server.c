#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <process.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096
#define MAX_CLIENTS 20

typedef struct ClientData
{
    SOCKET Listener;
} CLIENTDATA, *PCLIENTDATA;

char PORT[3];
PCLIENTDATA pDataArray[10];
DWORD dwThreadIdArray[10];
HANDLE hThreadArray[10];
int threadNbr = 0;
int clientNbr = 0;
SOCKET Clients[MAX_CLIENTS];

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

int init_client_childprocess(SOCKET ClientSocket){
    int iResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iSendResult;
    for (;;)
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            printf("%s", recvbuf);
            memset(recvbuf, 0, 4096);
            recvbuf[0] = '\0';
            fgets(recvbuf, 4096, stdin);
            iSendResult = send(ClientSocket, recvbuf, strlen(recvbuf), 0);
            memset(recvbuf, 0, 4096);
            recvbuf[0] = '\0';
            if (iSendResult == SOCKET_ERROR)
            {
                printf("Client unreachable : %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 0;
            }
        }else{
            printf("Client unreachable : %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 0;
        }
    }
}

DWORD WINAPI init_client(LPVOID lpParam)
{
    SOCKET ClientSocket = INVALID_SOCKET;
    PCLIENTDATA pData;
    pData = (PCLIENTDATA)lpParam;
    ClientSocket = accept(pData->Listener, NULL, NULL);
    printf("\n>>New client\n");
    closesocket(pData->Listener);
    
    SOCKET listener = init_listener();
    pDataArray[threadNbr] = (PCLIENTDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                           sizeof(PCLIENTDATA));
    if (pDataArray[threadNbr] == NULL)
    {
        ExitProcess(2);
    }
    pDataArray[threadNbr]->Listener = listener;
    init_client_thread(pDataArray[threadNbr]);
    
    Clients[clientNbr] = ClientSocket;
    clientNbr++;
}

void init_client_thread(PCLIENTDATA pclientdata)
{
    hThreadArray[threadNbr] = CreateThread(
        NULL,                         // default security attributes
        0,                            // use default stack size
        init_client,                  // thread function name
        pclientdata,                  // argument to thread function
        0,                            // use default creation flags
        &dwThreadIdArray[threadNbr]); // returns the thread identifier

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
        pDataArray[threadNbr] = (PCLIENTDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                               sizeof(PCLIENTDATA));
        if (pDataArray[threadNbr] == NULL)
        {
            ExitProcess(2);
        }
        pDataArray[threadNbr]->Listener = listener;
        init_client_thread(pDataArray[threadNbr]);
    }
    else if (choice == 2)
    {
        if(Clients[0] != NULL ){
            for(int i = 0; i < MAX_CLIENTS; i++){
                if(Clients[i] != NULL){
                    printf("Client %d connected.\n", i);
                }
            }
            printf("Choose a client : ");
            scanf("%d", &choice);
            init_client_childprocess(Clients[choice]);
        }else{
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