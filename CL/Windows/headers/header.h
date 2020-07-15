#ifndef _HEADER_H_
#define _HEADER_H_

#define _WIN32_WINNT 0x501
#define DEFAULT_BUFLEN 512

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

typedef struct Pipe0
{
    HANDLE HANDLES[4];
    int bufsize;
} * Pipe;
Pipe pipe;

char OUTPUT_text[4096];
DWORD OUTPUT_size;
char *IP;
char *PORT;

#include "multiprocess.h"
#include "socket.h"
#include "config.h"

#endif