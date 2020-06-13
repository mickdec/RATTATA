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
#define DEFAULT_PORT "8888"

void logprint(char *type,char *message){
    if(strcmp(type, "ERROR")){
        printf("[-]%s", message);
    }
}

void init_listener()
{
    logprint("ERROR","message");
    printf("oui");
    return 0;
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    iResult = listen(ListenSocket, SOMAXCONN);
    ClientSocket = accept(ListenSocket, NULL, NULL);
    closesocket(ListenSocket);

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
                init_listener();
            }
        }
    }
}

int __cdecl main(void)
{
    init_listener();
    return 0;
}