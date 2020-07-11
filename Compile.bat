rm SVW.exe
rm CLW.exe
gcc CL\Windows\client.c -lws2_32 -lpsapi -o CLW.exe
gcc SV\Windows\src\config.c SV\Windows\src\layout.c SV\Windows\src\threading.c SV\Windows\src\socket.c SV\Windows\main.c -lws2_32 -lpsapi -o SVW.exe -Wall -Wextra