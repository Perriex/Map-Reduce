#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

int mapperCount = 0;  // number of mappers
int reducerCount = 0; // number of reducers
int childsCount = 0;  // number of processes

int parentToMapperPipe[2];  // pipe between parent -> mapper
int reducerToParentPipe[2]; // pipe between parent <- reducer

// calculate number of files exits in folder "../testcases"
int countfiles(char *);
// init global variables
void initChildren(int);
// create pipes between parent and children
void createPipes(void);
// fork mappers and reducer (children)
void forkChildren(void);

int main()
{
    int fileCount = countfiles("../testcases");
    printf("Number of files: %d\n", fileCount);

    initChildren(fileCount);

    createPipes();

    forkChildren();

    return 0;
}

/* functions used in main */

// communicate with children
void assignProc(void);
// exec mapper
void createMapper(void);
//  exec reducer
void createReducer(void);

void createReducer(void)
{

}

void createMapper(void)
{

}

void assignProc(void)
{

}

void forkChildren(void)
{
    int parentPid = fork();
    if (parentPid < 0)
    {
        printf("Error in Fork\n");
        exit(1);
    }
    int pid = getpid();
    if (parentPid > 0)
    {
        printf("parent process with pid in map : %d\n", pid);

        if (childsCount < mapperCount + 1) //plus reducer => childrenCount
        {
            childsCount += 1;
            forkChildren();
        }
        else
        {
            assignProc();
            sleep(1); // wait until child forking done
            printf("Done!\n");
            wait(NULL); // all children finish their job
            exit(0);
        }
    }
    else
    {
        if (childsCount > mapperCount )
        {
            printf("All children forked.\n");
        }
        else
        {
            if (childsCount < mapperCount)
            {
                printf("Create Mapper with pid: %d\n", pid);
                createMapper();
            }
            else
            {
                printf("Create Reducer with pid: %d\n", pid);
                createReducer();
            }
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