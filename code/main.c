#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

int mapperCount = 0;  // number of mappers
int reducerCount = 0; // number of reducers
int childsCount = 0;

int parentToMapperPipe[2];  // pipe between parent -> mapper
int reducerToParentPipe[2]; // pipe between parent <- reducer

int countfiles(char *); // calculate number of files exits in folder "../testcases"
void initChildren(int); // init global variables
void createPipes(void); // create pipes between parent and children
void forkMappers(void); // fork children

int main()
{
    //first we calculate number of files in testcases
    int fileCount = countfiles("../testcases");
    printf("Number of files: %d\n", fileCount);

    //init number of processes
    initChildren(fileCount);

    // create pipes
    createPipes();

    // fork mappers
    forkMappers();

    return 0;
}

/* functions used in main */

void forkMappers(void)
{
    int parentPid = fork();
    if (parentPid < 0)
    {
        printf("Error in Fork parent\n");
        exit(1);
    }
    if (parentPid == 0)
    {
    }
    else
    {
        int pid = getpid();
        printf("new process with pid in map : %d", pid);

        if (childsCount < mapperCount)
        {
            childsCount += 1;
            forkMappers();
        }
        else
        {
        }
    }
}

void createPipes(void)
{
    if (pipe(parentToMapperPipe) < 0)
    {
        printf("Error in pipe parent-mapper\n");
        exit(1);
    }
    if (pipe(reducerToParentPipe) < 0)
    {
        printf("Error in pipe reducer-parent\n");
        exit(1);
    }
}

void initChildren(int fileCount)
{
    mapperCount = fileCount;
    reducerCount = 1;

    printf("Number of mappers : %d and number of reducers : %d \n", mapperCount, reducerCount);

    return;
}

int countfiles(char *path)
{
    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    if ((dir_ptr = opendir(path)) == NULL)
        return 0;

    int count = 0;
    while ((direntp = readdir(dir_ptr)))
    {
        if (strcmp(direntp->d_name, ".") == 0 ||
            strcmp(direntp->d_name, "..") == 0)
            continue;
        ++count;
    }
    closedir(dir_ptr);
    return count;
}