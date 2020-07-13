#include "headers/header.h"

int main()
{
    int conf_error;
    conf_error = read_config();
    if(conf_error){
        read_config();
    }

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)init_socket, NULL, 0, NULL);
    int a = 0;
    for (;;)
    {
        scanf("%d", &a);
    }
    return 0;
}