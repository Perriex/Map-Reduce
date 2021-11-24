#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

char words[1000][100] = {0};
int numWords[1000] = {0};

int wordsCount = 0;

int setWords(char *);
void writeToMain(int);

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
        fd = open(path, O_RDONLY);
        char *str = malloc(100 * sizeof(char));
        char *input = str;
        int index = -1;
        for (;;)
        {
            if (read(fd, input, 1) <= 0)
                continue;

            if (*input == ':')
            {
                *input = '\0';
                index = setWords(str);
                input = str;
                continue;
            }
            if (*input == '\0')
            {
                *input = '\0';
                numWords[index] += atoi(str);
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
        close(fd);
        count -= 1;
    }
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Mapper: Not enough arguments!\n");
        exit(1);
    }
    int readPipe = atoi(argv[0]);
    int writePipe = atoi(argv[1]);
    int tasks = atoi(argv[2]);

    close(readPipe);

    waitForMappers(tasks);

    writeToMain(writePipe);

    close(writePipe);

    exit(0);

    return 0;
}

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

int setWords(char *word)
{
    int index = getIndex(word);
    if (index < 0)
    {
        strcpy(words[wordsCount], word);
        numWords[wordsCount] = 0;
        wordsCount += 1;
        return wordsCount - 1;
    }
    else
        return index;
}

void writeToMain(int pipe)
{
    char temp;
    int count = 0;
    char *string = malloc(sizeof(char));

    while (count < wordsCount)
    {
        char *split = ":";
        char *num = intToSrc(numWords[count]);
        char *path = (char *)malloc(strlen(words[count]) + strlen(split) + strlen(num) + 2);

        strcpy(path, words[count]);
        strcat(path, split);
        strcat(path, num);
        strcat(path, "\n\0");
        write(pipe, path, strlen(path) + 1);
        count += 1;
    }
    string = "$";
    write(pipe, string, strlen(string) + 1);
    return;
}