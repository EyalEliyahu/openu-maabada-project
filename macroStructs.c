#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "macroStructs.h"

macroLine *macroHead = NULL;
macroLine *macroTail = NULL;

void freeMacroList()
{
    macroLine *temp;

    while (macroHead)
    {
        temp = macroHead->next;
        free(macroHead->contentLines);
        free(macroHead);
        macroHead = temp;
    }
}

void macroListAppend(char* macroName)
{
	macroLine* newMacro = (macroLine*)malloc(sizeof(macroLine));
    
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

macroLine *macroLineInList(char* macroName)
{
    macroLine *temp = macroHead;
    while (temp)
    {
        if (IS_STR_EQL(temp->macro, macroName))
            return temp;
        else
            temp = temp->next;
    }
    return NULL;
}

int macroExistsInList(char* macroName)
{
    macroLine *temp = macroHead;
    while (temp)
    {
        if (IS_STR_EQL(temp->macro, macroName))
            return TRUE;
        else
            temp = temp->next;
    }
    return FALSE;
}
