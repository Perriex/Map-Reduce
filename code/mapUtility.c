#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *createOutputFileMapper(char *file)
{
    char *output = "map_";

    char *name = (char *)malooc(strlen(output) + strlen(file));

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

void sortWords(char *file, int wordCount)
{
    FILE *fIn;
    fIn = fopen(file, "r");

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
        if (temp == ',')
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
    free(outputWords);
    i = 0;
    for (i = 0; i < nRows; i++)
    {
        free(arrayOfString[i]);
    }
    free(arrayOfString);
    return;
}

void countWords(char *file)
{
    FILE *fIn;
    fIn = fopen(file, "r");

    char temp;
    int charCount = 0;
    int wordCount = 0;

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

    sortWords(file, wordCount);
    return;
}