#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "reducerUtility.c"


void waitForMappers(int tasks){
    int fd;
    int count = tasks;
    char * myfifo = "/tmp/myfifo";

    mkfifo(myfifo,0666);

    char str[100];
    while(count != 0){
        count -= 1;
        fd = open(myfifo,O_RDONLY);
        read(fd,str,100);
        setWords(str);
        printf("File %s gotten\n",str);
        close(fd);
    }

    writeToFile();
}


int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Mapper: Not enough arguments!\n");
        exit(1);
    }
    char *readPipe = argv[1];
    char *writePipe = argv[2];
    int tasks = atoi(argv[3]);

    close(readPipe);

    waitForMappers(tasks);
    
    char *buf = "D\0";
    printf("Reduced file initilized!\n");
    write(writePipe,buf,strlen(buf));

    close(writePipe);

    return 0;
}

