#ifndef _HEADER_H_
#define _HEADER_H_

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8888"
#define DEFAULT_IP "127.0.0.1"

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Pipe0
{
    HANDLE HANDLES[4];
    int bufsize;
} * Pipe;
Pipe pipe;

char OUTPUT_text[4096];
DWORD OUTPUT_size;

#include "multiprocess.h"
#include "socket.h"
#include "config.h"

#endif