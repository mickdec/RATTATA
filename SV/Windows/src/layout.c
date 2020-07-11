#include "../headers/header.h"

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