rm OUTPUT/SVW.exe
rm OUTPUT/CLW.exe
gcc "C:\Users\micka\Desktop\RATTATA\SV\Windows\main.c" -lws2_32 -lpsapi -o OUTPUT/SVW.exe
gcc "C:\Users\micka\Desktop\RATTATA\CL\Windows\main.c" -lws2_32 -lpsapi -o OUTPUT/CLW.exe