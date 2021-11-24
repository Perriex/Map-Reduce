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
    //printf("Number of files: %d\n", fileCount);

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

void convertToCsv()
{
    FILE *fIn;
    fIn = fopen("out", "r");
    FILE *fOut;
    char *name = "output.csv";
    fOut = fopen(name, "w+");
    char temp;
    while ((temp = fgetc(fIn)) != EOF)
    {
        fprintf(fOut, "%c", temp);
    }
    fclose(fIn);
    fclose(fOut);
    printf("Output generated! name: output.csv\n");
}

void createReducer(void)
{
    char *snum = intToSrc(mapperCount);
    printf("exec reducer!\n");
    char *args[] = {intToSrc(reducerToParentPipe[0]), intToSrc(reducerToParentPipe[1]), snum, NULL};
    //printf("%s %s %s %s\n", args[0], args[1], args[2], args[3]);
    execv("./reducer", args);
}

void createMapper(void)
{
    // printf("exec Mapper!\n");
    char *args[] = {intToSrc(parentToMapperPipe[0]), intToSrc(parentToMapperPipe[1]), NULL};
    execv("./mapper", args);
}

char *getName(int num)
{
    char *output2 = "../testcases/";
    char *output = ".csv";

    char *snum = intToSrc(num);
    //  printf("NYMM: %s\n", snum);
    char *name = (char *)malloc(strlen(output2) + strlen(snum) + strlen(output));
    strcpy(name, output2);
    // printf("1ADDRESSS: %s\n", name);
    strcat(name, snum);
    //printf("2ADDRESSS: %s\n", name);
    strcat(name, output);
    // printf("3ADDRESSS: %s\n", name);
    name[strlen(name)] = '\0';
    // printf("4ADDRESSS: %s\n", name);

    return name;
}

void assignProc(void)
{
    //assign mappers
    close(parentToMapperPipe[0]);
    int count = mapperCount;
    while (count != 0)
    {
        // printf("Assigning Files\n");
        char *buf = getName(count);
        // printf("%s\n", buf);
        int i = write(parentToMapperPipe[1], buf, strlen(buf));
        // printf("status: %d\n", i);
        count -= 1;

        sleep(1);
    }
    close(parentToMapperPipe[1]);

    // wair for reducer
    close(reducerToParentPipe[1]);

    char *buf = (char *)calloc(0, sizeof(char));
    int temp;

    while ((temp = read(reducerToParentPipe[1], buf, 1)) > 0)
    {
        printf("Done signal !\n");
        if (buf[0] == 'D')
        {
            //write in file .csv
            convertToCsv();
            break;
        }
    }

    free(buf);
    close(reducerToParentPipe[0]);
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
        //printf("parent process with pid in map : %d\n", pid);

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
                //   printf("Create Mapper with pid: %d\n", pid);
                createMapper();
            }
            else
            {
                //  printf("Create Reducer with pid: %d\n", pid);
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

    //printf("Number of mappers : %d and number of reducers : %d \n", mapperCount, reducerCount);

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