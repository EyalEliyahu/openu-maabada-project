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

	if (!newMacro) {
		fprintf(stderr, "Memory allocation for new macro failed!");
		exit(1);
	}
	
	strcpy(newMacro->macro,macroName);
    newMacro->numOfContentLines = 0;
    newMacro->contentLines = malloc(sizeof(char) * MAX_LINE_LENGTH);

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

macroLine *macroLineInList(char *macroName)
{
    macroLine *temp = macroHead;
    while (temp)
    {
        if (strcmp(temp->macro, macroName) == 0)
            return temp;
        else
            temp = temp->next;
    }
    return NULL;
}

int macroExistsInList(char *macroName)
{
    macroLine *temp = macroHead;
    while (temp)
    {
        if (strcmp(temp->macro, macroName) == 0)
            return TRUE;
        else
            temp = temp->next;
    }
    return FALSE;
}

void printMacroList() /* Used for debugging */
{
    int i = 0;
    macroLine *temp = macroHead;
    printf("----------------------- MACRO list START ---------------------\n");
    while (temp)
    {
		printf("Macro: %s\n", temp->macro);
        printf("Content:\n");
        while (i < temp->numOfContentLines)
		{
			printf("\t%s", temp->contentLines[i++]);
		}
		temp = temp->next;
        i = 0;
    }
    printf("----------------------- MACRO list END ---------------------\n");
}
