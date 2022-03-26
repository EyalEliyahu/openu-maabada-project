#ifndef _MACRO_H
#define _MACRO_H

int processMacroLine(char* lineContent, int* isInMacro, char* macroString, char* file, FILE* amFilePtr);
int translateMacros(FILE* assemblyFile, char* fileName);
#endif
