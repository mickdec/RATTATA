#ifndef _HEADER_H_
#define _HEADER_H_

#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_PORT "8888"
#define DEFAULT_BUFLEN 4096
#define MAX_CLIENTS 20

typedef struct ClientInfo
{
    int id;
    char *ip;
    SOCKET Socket;
} CLIENTINFO, *PCLIENTINFO;

typedef struct ThreadData
{
    SOCKET Listener;
    char* iport;
} THREADDATA, *PTHREADDATA;

PCLIENTINFO PClients[MAX_CLIENTS];
DWORD dwThreadIdArray[MAX_CLIENTS];
HANDLE hThreadArray[MAX_CLIENTS];
PTHREADDATA pDataArray[MAX_CLIENTS];
int THREADNBR;

#include "threading.h"
#include "socket.h"
#include "layout.h"
#include "config.h"

#endif