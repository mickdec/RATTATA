#include "../headers/header.h"

int read_config(){
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
        return 0;
    }

    while(!feof(fp))
    {
        fread(buffer, 60, 1, fp);
    }
    fclose(fp);

    if(buffer[0] != 'I' || buffer[1] != 'P' || buffer[2] != '='){
        remove(file_name);
        read_config();
        return 0;
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
        return 0;
    }

    for(int i = bufferSize+1; i<bufferSize+13; i++){
        if(buffer[i] == '\n'){
            buffer[i] = '\0';
            break;
        }
    }

    int c = 0;
    for(int i = bufferSize+1; i<strlen(buffer); i++){
        if(i > bufferSize+5){
            TMPPORTbuffer[c] = buffer[i];
            c++;
            if(c == 5){
                TMPPORTbuffer[c+1] = '\0';
                break;
            }
        }
    }

    PORT = malloc(strlen(buffer)*sizeof(char));
    strcpy(PORT, TMPPORTbuffer);
    return 0;
}