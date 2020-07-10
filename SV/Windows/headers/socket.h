#ifndef _SOCKET_H_
#define _SOCKET_H_

SOCKET init_listener(char *PORT);
DWORD WINAPI init_client(LPVOID lpParam);

#endif