gcc "SV\Windows\main.c" -lws2_32 -lpsapi -o OUTPUT/SVW_Windows.exe
py.exe .\Obfuscate.py 
gcc "CL\Windows\GeneratedC_Obfuscated_source.c" -lws2_32 -lpsapi -o OUTPUT/CLW_Windows.exe