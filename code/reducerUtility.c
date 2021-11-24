#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    char *string = calloc(0, sizeof(char));

    while ((temp = fgetc(fIn)) != EOF)
    {
        if (temp = '\n')
        {
            if (charCount > 0)
            {
                charCount = 0;

                char *word = strtok(string, ":");
                char *num = strtok(string, ":");
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

                free(string);
                string = calloc(0, sizeof(char));
            }
        }
        else
        {
            if (charCount == 0)
            {
                frea(string);
                string = calloc(0, sizeof(char));
            }
            string[charCount] = temp;
            charCount++;
        }
    }

    free(string);
    fclose(fIn);
}


void writeToFile(){
    FILE *fOut;
    char *name = "out";
    fOut = fopen(name, "w+");

    for(int i=0;i<=wordsCount;i++){
        printf("result for %s -> %d\n", words[i], numWords[i]);
        fprintf(fOut, "%s:%d\n", words[i], numWords[i]);
    }

    free(name);
    fclose(fOut);
}