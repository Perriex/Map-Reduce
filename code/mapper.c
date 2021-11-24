#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mapUtility.c"

char* map(char *file)
{
    return handleFile(file);
}

void createPipeToReduce(char *name)
{
    printf("Opreration done! send to Reducer!\n");
    char *fifo = "/tmp/myfifo";

    mkfifo(fifo,0666);

    int fd = open(fifo,O_WRONLY);

    printf("File %s generated!\n",name);
    write(fd,name,strlen(name)+1);
    close(fd);
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Mapper: Not enough arguments!\n");
        exit(1);
    }
    char *readPipe = argv[1];
    char *writePipe = argv[2];
    close(writePipe);

    char *buf = (char *)calloc(0, sizeof(char));

    int temp;

    while ((temp = read(readPipe, buf, 1000)) > 0)
    {
        printf("Mapper with pid : %d read from pip: %s\n", getpid(), buf);

        char* name = map(buf);

        createPipeToReduce(name);
    }

    printf("@ Mapper with pid %d finished!\n", getpid());
    free(buf);
    close(readPipe);

    return 0;
}