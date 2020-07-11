#include "../headers/header.h"

SOCKET init_listener()
{
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, PORT, &hints, &result);
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    listen(ListenSocket, SOMAXCONN);

    return ListenSocket;
}

DWORD WINAPI init_client(LPVOID lpParam)
{
    SOCKET ClientSocket = INVALID_SOCKET;
    PTHREADDATA pData;
    pData = (PTHREADDATA)lpParam;
    
    ClientSocket = accept(pData->Listener, NULL, NULL);
    closesocket(pData->Listener);

    SOCKET listener = init_listener();
    
    pDataArray[THREADNBR] = (PTHREADDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PTHREADDATA));
    if (pDataArray[THREADNBR] == NULL)
    {
        ExitProcess(2);
    }
    pDataArray[THREADNBR]->Listener = listener;
    init_client_thread(pDataArray[THREADNBR]);

    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        if (PClients[i]->id == 0)
        {
            PClients[i]->id = i;
            strcpy(PClients[i]->ip, "0.0.0.0");
            PClients[i]->Socket = ClientSocket;
            printf("\nA wild rattata appeared - ID=%d IP=%s\n", PClients[i]->id, PClients[i]->ip);
            break;
        }
    }

    return 0;
}