#include "../headers/header.h"

int read_config()
{
    char *content = "PORT=8888";
    char *file_name = "options.cfg";

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

    char buffer[11];
    char port[5];
    while(!feof(fp))
    {
        fread(buffer, 11, 1, fp);
    }
    fclose(fp);

    buffer[11] = '\0';
    
    int i = 0;
    while(isdigit((int)buffer[i+5])){
        port[i] = buffer[i+5];
        i++;
        if(i == 5){
            break;
        }
    }
    port[i] = '\0';
    strcpy(PORT, port);
    return 0;
}