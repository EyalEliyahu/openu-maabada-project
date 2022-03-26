#ifndef _UTILS_H
#define _UTILS_H

#include "consts.h"
#include "globalStructs.h"
#include "symbolTable.h"
#include "optCodeData.h"

int isStringInteger(char* string);
int isAlphanumericStr(char* string);
void *safeMalloc(long size);
char* stringsConcat(char* string1, char* string2);
void printLineError(int lineIndex, char* message, ...);

int fetchType(char* lineContent, int* i);
int fetchOperands(int lineIndex, char* lineContent, int i, char* *operandsArray, int* numOfOperands);
int openFileSafe(FILE** fileStream, char* fileName, char* fileExt, char* openMethod);
codeInstruction *generateFirstCodeWord(optCodeData *opcodeData);
codeInstruction *generateSecondCodeWord(int lineIndex, char* lineContent, optCodeData *opcodeData, char* *operandsArray, int numOfOperands);
int getSymbolFromLine(int lineIndex, char* lineContent, char* symbolDest);
int calcIcBase(int ic);
int calcIcOffset(int ic);
int validateStringEntry(int lineIndex, char* lineContent, int i);


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
