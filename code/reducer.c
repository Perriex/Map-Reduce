#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

void setWords(char *);
void writeToFile(void);

char *intToSrc(int value)
{
    char *buffer = calloc(0, sizeof(char));
    snprintf(buffer, 10, "%d", value);
    return buffer;
}

void waitForMappers(int tasks)
{
    int fd;
    int count = tasks;
    char *fifo = "/tmp/c_sort_";

    while (count > 0)
    {
        char *numFile = intToSrc(count);
        char *path = (char *)malloc(strlen(fifo) + strlen(numFile));

        strcpy(path, fifo);
        strcat(path, numFile);

        mkfifo(path, 0666);
        printf("@@ for pipe : %s\n", path);
        fd = open(path, O_RDONLY);
        char *str = malloc(100 * sizeof(char));
        char *input=str;
        printf("end\n");
        for (;;)
        {
            if(read(fd, input, 1) <= 0)
                continue;
                
            if(*input == ':' || *input == '\0')
            {
                *input = '\0';
                printf("ins: %s\n", str);
                input = str;
                continue;
            }
            //setWords(str);
            if (*input == '$')
            {
                break;
            }
            input++;
        }
        free(str);
        close(fd);
        count -= 1;
    }

    // writeToFile();
}

int main(int argc, char *argv[])
{
    // printf("_____$$$$$$_________IN REDUCER__________$$$$$$_____\n");

    if (argc != 3)
    {
        // printf("Mapper: Not enough arguments!\n");
        exit(1);
    }
    int readPipe = atoi(argv[0]);
    int writePipe = atoi(argv[1]);
    int tasks = atoi(argv[2]);

    close(readPipe);

    waitForMappers(tasks);

    char *buf = "D\0";
    // printf("Reduced file initilized!\n");
    write(writePipe, buf, strlen(buf));

    close(writePipe);

    return 0;
}

char words[1000][100] = {0};
int numWords[1000] = {0};

int wordsCount = 0;

int getIndex(char *word)
{
    for (int i = 0; i <= wordsCount; i++)
    {
        if (strcmp(words[i], word) == 0)
        {
            return i;
        }
    }
    return -1;
}

void setWords(char *file)
{
    FILE *fIn;
    fIn = fopen(file, "r");

    char temp;
    int charCount = 0;
    char *string = malloc(sizeof(char));

    while ((temp = fgetc(fIn)) != EOF)
    {
        if (temp = '\n')
        {
            if (charCount > 0)
            {
                printf("word : %s\n", string);
                charCount = 0;

                char *word = strtok(string, ":");
                char *num = strtok(NULL, ":");
                printf("word, num : %s %s\n", word, num);
                int index = getIndex(word);
                if (index < 0)
                {
                    strcpy(words[wordsCount], word);
                    numWords[wordsCount] = atoi(num);
                    wordsCount += 1;
                }
                else
                {
                    numWords[wordsCount] += atoi(num);
                }

                printf("HERE1\n");
                free(string);
                printf("HERE1b\n");
                string = calloc(0, sizeof(char));
            }
        }
        else
        {
            if (charCount == 0)
            {
                printf("HERE2\n");
                free(string);
                printf("HERE2b\n");
                string = calloc(0, sizeof(char));
            }
            string[charCount] = temp;
            charCount++;
        }
    }

    printf("HERE3\n");
    free(string);
    printf("HERE3b\n");
    fclose(fIn);
}

void writeToFile()
{
    FILE *fOut;
    char *name = "out";
    fOut = fopen(name, "w+");

    for (int i = 0; i <= wordsCount; i++)
    {
        // printf("result for %s -> %d\n", words[i], numWords[i]);
        fprintf(fOut, "%s:%d\n", words[i], numWords[i]);
    }

    free(name);
    fclose(fOut);
}