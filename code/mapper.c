#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

char *handleFile(char *);

char *map(char *file)
{
    return handleFile(file);
}

void createPipeToReduce(char *name)
{
    char *fifo = "/tmp/";

    char *path = (char *)malloc(strlen(name) + strlen(fifo));

    strcpy(path, fifo);
    strcat(path, name);

    mkfifo(path, 0666);

    FILE *fIn;
    fIn = fopen(name, "r");

    char temp;
    int count = 0;
    char *string = malloc(sizeof(char));
    int fd = open(path, O_WRONLY);

    while ((temp = fgetc(fIn)) != EOF)
    {
        if (temp == '\n')
        {
            if (count > 0)
            {
                int fd = open(path, O_WRONLY);
                string[count] = '\0';
                write(fd, string, strlen(string) + 1);
                count = 0;
            }
        }
        else
        {
            if (count == 0)
            {
                free(string);
                string = calloc(100, sizeof(char));
            }
            string[count] = temp;
            count++;
        }
    }
    string = "$";
    write(fd, string, strlen(string) + 1);
    close(fd);

}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Mapper: Not enough arguments!\n");
        exit(1);
    }

    int readPipe = atoi(argv[0]);
    int writePipe = atoi(argv[1]);
    close(writePipe);

    char *buf = (char *)calloc(1000, sizeof(char));

    int temp;

    while ((temp = read(readPipe, buf, 1000)) > 0)
    {
        char *name = map(buf);

        createPipeToReduce(name);
    }

    printf("@ Mapper with pid %d finished!\n", getpid());
    free(buf);
    close(readPipe);
    exit(0);
    return 0;
}

char *createOutputFileMapper(char *file)
{
    char *output = "sort_";

    char *name = (char *)malloc(strlen(output) + strlen(file));

    strcpy(name, output);
    strcat(name, file);

    name[strlen(name)] = '\0';
    return name;
}

char *createOutputCountMapper(char *file)
{
    char *output = "c_";

    char *name = (char *)malloc(strlen(output) + strlen(file));

    strcpy(name, output);
    strcat(name, file);

    name[strlen(name)] = '\0';
    return name;
}

char **sortArray(int nRows, char **arrayOfString)
{
    char *tempString = malloc(sizeof(char));
    int i = 0;
    for (i = 0; i < nRows; i++)
    {
        int j = 0;
        for (j = 1; j < nRows; j++)
        {
            if (strcmp(arrayOfString[j - 1], arrayOfString[j]) > 0)
            {
                free(tempString);
                tempString = calloc(0, sizeof(char));
                memcpy(tempString, arrayOfString[j - 1], strlen(arrayOfString[j - 1]));

                free(arrayOfString[j - 1]);
                arrayOfString[j - 1] = calloc(0, sizeof(char));
                memcpy(arrayOfString[j - 1], arrayOfString[j], strlen(arrayOfString[j]));

                free(arrayOfString[j]);
                arrayOfString[j] = calloc(0, sizeof(char));
                memcpy(arrayOfString[j], tempString, strlen(tempString));
            }
        }
    }
    free(tempString);

    return arrayOfString;
}

char *sortWords(char *file, int wordCount, char *addr)
{
    FILE *fIn;
    fIn = fopen(addr, "r");

    char temp;
    char *string = malloc(sizeof(char));
    int count = 0;

    int nRows = wordCount;
    int recordCount = 0;
    char **arrayOfString = malloc(nRows * sizeof(char *));
    int i = 0;
    for (i = 0; i < nRows; i++)
    {
        arrayOfString[i] = malloc(sizeof(char));
    }

    while ((temp = fgetc(fIn)) != EOF)
    {
        if (temp == ',' || temp == '\n')
        {
            if (count > 0)
            {
                arrayOfString[recordCount] = calloc(0, sizeof(char));

                memcpy(arrayOfString[recordCount], string, strlen(string));
                arrayOfString[recordCount][strlen(arrayOfString[recordCount])] = '\0';

                recordCount++;
                count = 0;
                free(string);
                string = calloc(0, sizeof(char));
            }
        }
        else
        {
            if (count == 0)
            {
                free(string);
                string = calloc(0, sizeof(char));
            }
            string[count] = temp;
            count++;
        }
    }

    free(string);
    fclose(fIn);

    arrayOfString = sortArray(nRows, arrayOfString);

    FILE *fOut;
    char *outputWords = createOutputFileMapper(file);
    fOut = fopen(outputWords, "w+");
    i = 0;
    for (i = 0; i < nRows; i++)
    {
        arrayOfString[i][strlen(arrayOfString[i])] = '\0';
        fprintf(fOut, "%s\n", arrayOfString[i]);
    }
    fclose(fOut);
    i = 0;
    for (i = 0; i < nRows; i++)
    {
        free(arrayOfString[i]);
    }
    free(arrayOfString);
    return outputWords;
}

char *countWords(char *file)
{
    FILE *fIn;
    fIn = fopen(file, "r");

    char temp;
    int charCount = 0;
    char *string = calloc(0, sizeof(char));

    char *prevString = calloc(0, sizeof(char));

    int matchCount = 1;

    FILE *fOut;
    char *name = createOutputCountMapper(file);
    fOut = fopen(name, "w+");

    while ((temp = fgetc(fIn)) != EOF)
    {
        if (temp == '\n')
        {
            if (charCount > 0)
            {
                charCount = 0;
                if (strlen(prevString) <= 0)
                {
                    string[strlen(string)] = '\0';

                    strcpy(prevString, string);
                    prevString[strlen(prevString)] = '\0';
                }
                else
                {
                    if (strcmp(prevString, string) != 0)
                    {
                        prevString[strlen(prevString)] = '\0';

                        fprintf(fOut, "%s:%d\n", prevString, matchCount);
                        matchCount = 1;
                        string[strlen(string)] = '\0';

                        strcpy(prevString, string);
                        prevString[strlen(prevString)] = '\0';
                    }
                    else
                    {
                        matchCount += 1;
                    }
                }

                free(string);
                string = calloc(0, sizeof(char));
            }
        }
        else
        {
            if (charCount == 0)
            {
                free(string);
                string = calloc(0, sizeof(char));
            }
            string[charCount] = temp;
            charCount++;
        }
    }
    fprintf(fOut, "%s:%d\n", prevString, matchCount);

    free(string);
    free(prevString);
    fclose(fIn);
    fclose(fOut);

    return name;
}

char *handleFile(char *file)
{
    FILE *fIn;
    fIn = fopen(file, "r");
    char *tAddr = (char *)malloc(strlen(file));
    strcpy(tAddr, file);
    char temp;
    int charCount = 0;
    int wordCount = 1;

    while ((temp = fgetc(fIn)) != EOF)
    {
        if (temp == ',')
        {
            if (charCount > 0)
            {
                charCount = 0;
                wordCount += 1;
            }
        }
        else
        {
            charCount += 1;
        }
    }
    fclose(fIn);
    char *nameFile = strtok(file, "/");
    char *nameFile2 = strtok(NULL, "/");
    char *nameFile3 = strtok(NULL, ".");
    char *outputFile = sortWords(nameFile3, wordCount, tAddr);

    char *nameMapFile = countWords(outputFile);

    return nameMapFile;
}