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
    //printf("Opreration done! send to Reducer!\n");
    char *fifo = "/tmp/myfifo";

    mkfifo(fifo, 0666);

    int fd = open(fifo, O_WRONLY);

    //printf("File %s generated!\n", name);
    write(fd, name, strlen(name) + 1);
    close(fd);
    exit(0);
}

int main(int argc, char *argv[])
{
    // printf("___________IN MAP_________\n");
    if (argc != 2)
    {
        printf("Mapper: Not enough arguments!\n");
        exit(1);
    }
    // printf("argument: %s %s\n", argv[1], argv[0]);

    int readPipe = atoi(argv[0]);
    int writePipe = atoi(argv[1]);
    // printf("argument In map: %d %d\n", readPipe, writePipe);
    close(writePipe);

    char *buf = (char *)calloc(0, sizeof(char));

    int temp;

    while ((temp = read(readPipe, buf, 1000)) > 0)
    {
        // printf("Mapper with pid : %d read from pip: %s\n", getpid(), buf);

        char *name = map(buf);

        createPipeToReduce(name);
    }

    printf("@ Mapper with pid %d finished!\n", getpid());
    free(buf);
    close(readPipe);

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
    //printf("ARRAY sort:%s %d\n", file, wordCount);

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
        //printf("char sort:%c \n", temp);
        if (temp == ',' || temp == '\n')
        {
            //printf("string sort:%s \n", string);
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
    //printf("addressout : %s\n", outputWords);
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
        //printf("NAME : %c\n", temp);
        if (temp == '\n')
        {
            //printf("HERE:\n");
            if (charCount > 0)
            {
                //printf("string %s and %s\n", prevString, string);
                charCount = 0;
                if (strlen(prevString) <= 0)
                {
                    string[strlen(string)] = '\0';
                    //printf("First Record: %s\n", string);

                    strcpy(prevString, string);
                    prevString[strlen(prevString)] = '\0';
                }
                else
                {
                    if (strcmp(prevString, string) != 0)
                    {
                        prevString[strlen(prevString)] = '\0';
                        // printf("No more word %s [%d]", prevString, matchCount);

                        fprintf(fOut, "%s:%d\n", prevString, matchCount);
                        matchCount = 1;
                        string[strlen(string)] = '\0';

                        strcpy(prevString, string);
                        prevString[strlen(prevString)] = '\0';
                    }
                    else
                    {
                        matchCount += 1;
                        // printf("match found for %s\n", string);
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
            //printf("CHAR : %c\n", temp);
            string[charCount] = temp;
            charCount++;
        }
    }
    if (matchCount == 1)
    {
        //printf("no match found for %s\n", prevString);
        fprintf(fOut, "%s:%d\n", prevString, matchCount);
    }
    else
    {
        //printf("match found for %s -> %d\n", prevString, matchCount);
        fprintf(fOut, "%s:%d\n", prevString, matchCount);
    }
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
    //printf("word count : %d\n",wordCount);
    char *nameFile = strtok(file, "/");
    char *nameFile2 = strtok(NULL, "/");
    char *nameFile3 = strtok(NULL, ".");
    //printf("name output: %s\n",nameFile3);
    char *outputFile = sortWords(nameFile3, wordCount, tAddr);

    char *nameMapFile = countWords(outputFile);

    return nameMapFile;
}