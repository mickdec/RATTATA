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

typedef struct ClientData {
    SOCKET Listener;
} CLIENTDATA, *PCLIENTDATA;

char PORT[3];
PCLIENTDATA pDataArray[10];
DWORD dwThreadIdArray[10];
HANDLE hThreadArray[10];
int threadNbr = 0;

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

DWORD WINAPI init_client(LPVOID lpParam){
    PCLIENTDATA pDataArray;
    pDataArray = (PCLIENTDATA)lpParam;

    SOCKET ClientSocket = INVALID_SOCKET;
    int iResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iSendResult;
    ClientSocket = accept(pDataArray->Listener, NULL, NULL);
    printf("New client\n");
    closesocket(pDataArray->Listener);
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
        }
    }
}

void start_client_thread(PCLIENTDATA pclientdata){
    hThreadArray[threadNbr] = CreateThread(
        NULL,                 // default security attributes
        0,                    // use default stack size
        init_client,   // thread function name
        pclientdata,                 // argument to thread function
        0,                    // use default creation flags
        &dwThreadIdArray[threadNbr]); // returns the thread identifier

    if (hThreadArray[threadNbr] == NULL)
    {
        printf("Error");
        ExitProcess(3);
    }
    threadNbr++;
}

int __cdecl main(void)
{
    int dummy;
    printf("RATTATA >>\n");
    printf("Listening PORT : ");
    scanf("%s", &PORT);
    // printf("Press ENTER to start the listening server..\n");
    // scanf("%d", &dummy);
    // printf("Server started.\n");
    // start_listening_thread();

    // while(1){
    //     printf("");
    // }
    
    SOCKET listener = init_listener();

    pDataArray[0] = (PCLIENTDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                           sizeof(PCLIENTDATA));
    if (pDataArray[0] == NULL)
    {
        ExitProcess(2);
    }

    pDataArray[0]->Listener = listener;

    start_client_thread(pDataArray[0]);

    printf("Press ENTER to start the listening server..\n");
    scanf("%d", &dummy);

    pDataArray[1] = (PCLIENTDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                           sizeof(PCLIENTDATA));
    if (pDataArray[1] == NULL)
    {
        ExitProcess(2);
    }

    pDataArray[1]->Listener = listener;

    start_client_thread(pDataArray[1]);

    while(1){
        printf("");
    }

    return 0;
}