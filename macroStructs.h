#include "utils.h"
#include "consts.h"
#include "assemblyStructures.h"
#ifndef _MACROSTRUCTS_H
#define _MACROSTRUCTS_H


typedef struct macroLine {
	char macro[MAX_SYMBOL_SIZE];
	int numOfContentLines;
	char** contentLines;
	struct macroLine *next;
} macroLine;

void freeMacroList();
void macroListAppend(char* macroName);
macroLine *macroLineInList(char *macroName);
int macroExistsInList(char *macroName);

extern macroLine *macroHead;
extern macroLine *macroTail;

#endif
