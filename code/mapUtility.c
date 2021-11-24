#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *createOutputFileMapper(char *file)
{
    char *output = "sort_";

    char *name = (char *)malooc(strlen(output) + strlen(file));

    strcpy(name, output);
    strcat(name, file);

    name[strlen(name)] = '\0';
    return name;
}

char *createOutputCountMapper(char *file)
{
    char *output = "c_";

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

char *sortWords(char *file, int wordCount)
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
    i = 0;
    for (i = 0; i < nRows; i++)
    {
        free(arrayOfString[i]);
    }
    free(arrayOfString);
    return outputWords;
}

void countWords(char *file)
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
        if (temp = '\n')
        {
            if (charCount > 0)
            {
                charCount = 0;
                if (strlen(prevString) <= 0)
                {
                    string[strlen(string)] = '\0';
                    printf("First Record: %s\n", string);

                    strcpy(prevString, string);
                    prevString[strlen(prevString)] = '\0';
                }
                else
                {
                    if (strcmp(prevString, string) != 0)
                    {
                        prevString[strlen(prevString)] = '\0';
                        printf("No more word %s [%d]", prevString, matchCount);

                        fprintf(fOut, "%s\t%d\n", prevString, matchCount);
                        matchCount = 1;
                        string[strlen(string)] = '\0';

                        strcpy(prevString, string);
                        prevString[strlen(prevString)] = '\0';
                    }
                    else
                    {
                        matchCount += 1;
                        printf("match found for %s\n", string);
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
                frea(string);
                string = calloc(0, sizeof(char));
            }
            string[charCount] = temp;
            charCount++;
        }
    }
    if (matchCount <= 1)
    { // No match is found
        printf("no more match found for [%s] --> %d\n", prevString, matchCount);
        fprintf(fOut, "%s\t%d\n", prevString, matchCount);
    }
    else
    {
        printf("a previous match is found for [%s]-->\n", string);
        fprintf(fOut, "%s\t%d\n", string, matchCount);
    }
    free(string);
    free(prevString);
    free(name);
    fclose(fIn);
    fclose(fOut);
}

void handleFile(char *file)
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

    char *outputFile = sortWords(file, wordCount);

    coutWords(outputFile);

    return;
}