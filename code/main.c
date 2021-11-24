#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

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

    initChildren(fileCount);

    createPipes();

    forkChildren();

    return 0;
}

/* functions used in main */

char *intToSrc(int value)
{
    char *buffer = calloc(0, sizeof(char));
    snprintf(buffer, 10, "%d", value);
    return buffer;
}

void createReducer(void)
{
    char *snum = intToSrc(mapperCount);
    printf("exec reducer!\n");
    char *args[] = {intToSrc(reducerToParentPipe[0]), intToSrc(reducerToParentPipe[1]), snum, NULL};
    execv("./reducer", args);
}

void createMapper(void)
{
    char *args[] = {intToSrc(parentToMapperPipe[0]), intToSrc(parentToMapperPipe[1]), NULL};
    execv("./mapper", args);
}

char *getName(int num)
{
    char *output2 = "../testcases/";
    char *output = ".csv";

    char *snum = intToSrc(num);
    char *name = (char *)malloc(strlen(output2) + strlen(snum) + strlen(output));
    strcpy(name, output2);
    strcat(name, snum);
    strcat(name, output);
    name[strlen(name)] = '\0';

    return name;
}

void assignProc(void)
{
    //assign mappers
    close(parentToMapperPipe[0]);
    int count = mapperCount;
    while (count != 0)
    {
        char *buf = getName(count);
        int i = write(parentToMapperPipe[1], buf, strlen(buf));
        count -= 1;

        sleep(1);
    }
    close(parentToMapperPipe[1]);

    // wair for reducer
    close(reducerToParentPipe[1]);

    char *str = malloc(100 * sizeof(char));
    char *input = str;

    FILE *fOut;
    char *name = "output.csv";
    fOut = fopen(name, "w+");

    for (;;)
    {
        if (read(reducerToParentPipe[0], input, 1) <= 0)
            continue;

        if (*input == '\0')
        {
            *input = '\0';
            printf("@ %s\n", str);
            fprintf(fOut, "%s", str);
            input = str;
            continue;
        }
        if (*input == '$')
        {
            break;
        }
        input++;
    }
    free(str);
    fclose(fOut);
    close(reducerToParentPipe[0]);
    printf("Output generated! name: output.csv\n");
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
        if (childsCount > mapperCount)
        {
            printf("All children forked.\n");
        }
        else
        {
            if (childsCount < mapperCount)
            {
                createMapper();
            }
            else
            {
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