#ifndef _MACRO_H
#define _MACRO_H

int processMacroLine(int line, char *lineContent, int *inMacro, char *macro, char *file, FILE *amFilePtr);
int translateMacros(FILE *assemblyFile, char* fileName);
#endif
