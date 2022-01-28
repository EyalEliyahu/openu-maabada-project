#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"

FILE* preprocessInputFile(FILE* originalFileStream, char* tempFilePath)
{
    FILE* tempFileStream;
    char* lineReadBuffer;
    tempFileStream = fopen(tempFilePath, "w"); 

    if (!tempFileStream) {
        /*TBD ERROR*/
        printf("\nUnable to open temp file file.\n");
    }


    /*
     * Read line from source file and write to destination 
     * file after replacing given word.
     */ 
    while (!fgets(lineReadBuffer, MAX_LINE_LENGTH, originalFileStream))
    {
        
        // Replace all occurrence of word from current line
        replaceAll(buffer, oldWord, newWord);

        // After replacing write it to temp file.
        fputs(buffer, fTemp);
    }


    /* Close all files to release resource */
    fclose(fPtr);
    fclose(fTemp);


    /* Delete original source file */
    remove(path);

    /* Rename temp file as original file */
    rename("replace.tmp", path);

    printf("\nSuccessfully replaced all occurrences of '%s' with '%s'.", oldWord, newWord);

    return 0;
}



/**
 * Replace all occurrences of a given a word in string.
 */
void replaceAll(char *str, const char *oldWord, const char *newWord)
{
    char *pos, temp[BUFFER_SIZE];
    int index = 0;
    int owlen;

    owlen = strlen(oldWord);

    // Fix: If oldWord and newWord are same it goes to infinite loop
    if (!strcmp(oldWord, newWord)) {
        return;
    }


    /*
     * Repeat till all occurrences are replaced. 
     */
    while ((pos = strstr(str, oldWord)) != NULL)
    {
        // Backup current line
        strcpy(temp, str);

        // Index of current found word
        index = pos - str;

        // Terminate str after word found index
        str[index] = '\0';

        // Concatenate str with new word 
        strcat(str, newWord);
        
        // Concatenate str with remaining words after 
        // oldword found index.
        strcat(str, temp + index + owlen);
    }
}