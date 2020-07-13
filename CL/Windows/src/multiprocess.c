#include "../headers/header.h"

Pipe create_pipes()
{
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
}

PTSTR pipe_init()
{
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
}

int frompipe(HANDLE in, HANDLE ou)
{
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
}