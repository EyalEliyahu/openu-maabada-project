#ifndef _UTILS_H
#define _UTILS_H

#include "consts.h"
#include "globalStructs.h"
#include "symbolTable.h"
#include "assemblyStructures.h"

/*
 * TODO
 */
char** getLineContent(char* lineString);

/*
 * TODO
 */
void *safeMalloc(long size);

/*
 * TODO
 */
char* stringsConcat(char *string1, char* string2);

int openFileSafe(FILE** fileStream, char* fileName, char* fileExt, char* openMethod);

char* strExt(const char *s1, const char *s2);

int fetchSymbol(int line, char* lineContent, char *symbolDest);

#define FIND_NEXT_CHAR(string, index) \
        while (string[i] == ' ' || string[i] == '\t'|| string[i] == '\n')\
        {\
                ++index;\
        }

#define INCREASE_I_UNTILL_CHAR(string, char, index) \
        for (;string[(index)] != char; (++(index)))\
        ;\
        (++(index));

#define THERE_IS_SYMBOL\
        symbolName[0] != '\0'

void append(char* s, char c);
void printErrorMessage(int line, char* message, ...);
int fetchSymbol(int line, char* lineContent, char *symbolDest);
int fetchType(char *lineContent, int *i);
int fetchOperands(int line, char* lineContent, int i, char **operandsArray, int *numOfOperands);
codeWord *generateFirstCodeWord(assemblyStructure *opcodeData);
codeWord *generateSecondCodeWord(int line, char* lineContent, assemblyStructure *opcodeData, char **operandsArray, int numOfOperands);
int calculateBase(int ic);
int calculateOffset(int ic);
int validateString(int line, char *lineContent, int i);

#endif

