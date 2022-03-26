#ifndef _UTILS_H
#define _UTILS_H

#include "consts.h"
#include "globalStructs.h"
#include "symbolTable.h"
#include "optCodeData.h"

int fetchType(char* lineContent, int* i);
int fetchOperands(int line, char* lineContent, int i, char* *operandsArray, int* numOfOperands);
void *safeMalloc(long size);
char* stringsConcat(char* string1, char* string2);
int openFileSafe(FILE** fileStream, char* fileName, char* fileExt, char* openMethod);
int getSymbolFromLine(int line, char* lineContent, char* symbolDest);
void printLineError(int line, char* message, ...);
codeWord *generateFirstCodeWord(optCodeData *opcodeData);
codeWord *generateSecondCodeWord(int line, char* lineContent, optCodeData *opcodeData, char* *operandsArray, int numOfOperands);
int calcIcBase(int ic);
int calcIcOffset(int ic);
int validateStringEntry(int line, char* lineContent, int i);
int isStringInteger(char* string);

#define INCREASE_INDEX_UNTILL_NEXT_CHAR(string, index) \
        while (string[index] == ' ' || string[index] == '\t'|| string[index] == '\n')\
        {\
                index++;\
        }

#define INCREASE_INDEX_UNTILL_CHAR(string, char, index) \
        for (;string[(index)] != char; (++(index)))\
        ;\
        index++;


#define IS_WHITESPACES_CHAR(c)\
        (c == ' ' || c == '\t')

#define IS_NULLISH_CHAR(c)\
        (!c || c == '\n' || c == '\0'  || c == EOF)
        
#define IS_STRING_EXISTS(string)\
       string[0] != '\0'

#define IS_STR_EQL(stringA, stringB)\
       strcmp(stringA, stringB) == 0

#define IS_TRUE_CHAR(c)\
       !IS_NULLISH_CHAR(c) && !IS_WHITESPACES_CHAR(c)

#endif
