#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "macroStructs.h"

macroLine* macroHead = NULL;
macroLine* macroTail = NULL;

void freeMacroList()
{
    macroLine* currentLine;

    while (macroHead)
    {
        currentLine = macroHead->next;
        free(macroHead->contentLines);
        free(macroHead);
        macroHead = currentLine;
    }
}

void appendToMacroList(char* macroName)
{
	macroLine* newMacro = (macroLine*)safeMalloc(sizeof(macroLine));
    
	strcpy(newMacro->macro,macroName);
    newMacro->numOfContentLines = 0;
    newMacro->contentLines = safeMalloc(sizeof(char) * MAX_LINE_LENGTH);

	if(!macroHead) {
		macroHead = newMacro;
		macroTail = macroHead;
		}
	else {
		macroTail->next = newMacro;
		macroTail = macroTail->next;
	}
	macroTail->next = NULL;

	return;
}

macroLine* getMacroLine(char* macroName)
{
    macroLine* currentLine = macroHead;
    while (currentLine)
    {
        if (IS_STR_EQL(currentLine->macro, macroName))
            return currentLine;
        else
            currentLine = currentLine->next;
    }
    return NULL;
}
