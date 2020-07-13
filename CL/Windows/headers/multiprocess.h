#ifndef _MULTIPROCESS_H_
#define _MULTIPROCESS_H_

SECURITY_ATTRIBUTES sa;
STARTUPINFO si;
PROCESS_INFORMATION pi;

Pipe create_pipes();
PTSTR pipe_init();
int frompipe(HANDLE in, HANDLE ou);

#endif