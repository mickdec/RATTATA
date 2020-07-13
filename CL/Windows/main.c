#include "headers/header.h"

int main()
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)init_socket, NULL, 0, NULL);
    int a = 0;
    for (;;)
    {
        scanf("%d", &a);
    }
    return 0;
}