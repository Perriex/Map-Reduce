#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include "mapUtility.c"

void map(char* file){
    handleFile(file);
}

void createPipeToReduce(){
    
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Mapper: Not enough arguments!\n");
        exit(1);
    }
    char *readPip = argv[1];
    char *writePip = argv[2];
    close(writePip);

    char *buf = (char *)calloc(0, sizeof(char));

    int temp;

    while ((temp = read(readPip, buf, 1000)) > 0)
    {
        printf("Mapper with pid : %d read from pip: %s\n", getpid(), buf);
        
        map(buf);

        createPipeToReduce();
    }

    printf("@ Mapper with pid %d finished!\n",getpid());
    free(buf);
    close(readPip);

    return 0;
}