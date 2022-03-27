#include "utils.h"
#include "consts.h"
#include "optCodeData.h"
#ifndef _MACROSTRUCTS_H
#define _MACROSTRUCTS_H

/* define macro line type*/
typedef struct macroLine {
	char macro[MAX_SYMBOL_SIZE];
	int numOfContentLines;
	char** contentLines;
	struct macroLine* next;
} macroLine;

void freeMacroList();
void appendToMacroList(char* macroName);
macroLine* getMacroLine(char* macroName);

extern macroLine* macroHead;
extern macroLine* macroTail;

#endif
