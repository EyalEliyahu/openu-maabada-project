#ifndef _UTILS_H
#define _UTILS_H

#include "consts.h"
#include "globalStructs.h"
#include "symbolTable.h"
#include "assemblyStructures.h"

int fetchType(char *lineContent, int *i);
int fetchOperands(int line, char* lineContent, int i, char **operandsArray, int *numOfOperands);
void *safeMalloc(long size);
char* stringsConcat(char *string1, char* string2);
int openFileSafe(FILE** fileStream, char* fileName, char* fileExt, char* openMethod);
int fetchSymbol(int line, char* lineContent, char *symbolDest);
void printErrorMessage(int line, char* message, ...);
codeWord *generateFirstCodeWord(assemblyStructure *opcodeData);
codeWord *generateSecondCodeWord(int line, char* lineContent, assemblyStructure *opcodeData, char **operandsArray, int numOfOperands);
int calculateBase(int ic);
int calculateOffset(int ic);
int validateString(int line, char *lineContent, int i);
int isInteger(char *string);

#define INCREASE_I_UNTILL_NEXT_CHAR(string, index) \
        while (string[index] == ' ' || string[index] == '\t'|| string[index] == '\n')\
        {\
                index++;\
        }

#define INCREASE_I_UNTILL_CHAR(string, char, index) \
        for (;string[(index)] != char; (++(index)))\
        ;\
        index++;

#define IS_STRING_EXISTS(symbolName)\
        symbolName[0] != '\0'
#endif
