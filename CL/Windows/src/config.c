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
}