#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "macroStructs.h"

/* This function handles the macro replacement */
int processMacroLine(int line, char *lineContent, int *isInMacro, char *macro, char *file, FILE *amFilePtr) {
	int indexInLine=0, j=0, k=0;
	char firstWord[MAX_LINE_LENGTH + 2];
	macroLine *temp;
	char* lineInMacro = "";
	/* look for the next char that is not whitespace/tab/newline */
	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
	/* Get the line content after removing spaces and tabs from the beginning until reach whitespace/tab/newline */
	for (; !IS_NULLISH_CHAR(lineContent[indexInLine]) && !IS_WHITESPACES_CHAR(lineContent[indexInLine]) && indexInLine <= MAX_LINE_LENGTH; indexInLine++, j++)
		firstWord[j] = lineContent[indexInLine];
	firstWord[j] = '\0';

	/* Check if the line contains macro that we already saved */
	if(macroExistsInList(firstWord) && !*isInMacro) {
		temp = macroLineInList(firstWord);
		/* replace macro name with macro content */
		while (k < temp->numOfContentLines)
		{
			fprintf(amFilePtr, "%s", temp->contentLines[k++]);
		}
		return TRUE;
	}

	indexInLine = 0;
	j = 0;
	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine); 
	/* Checks if this line is start of new macro */
    if (strncmp("macro", lineContent+indexInLine, 5) == 0 && !*isInMacro) {
		/* go to the end of the word: macro */
		INCREASE_INDEX_UNTILL_CHAR(lineContent, 'o', indexInLine);
		INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
		for (; !IS_NULLISH_CHAR(lineContent[indexInLine]) && !IS_WHITESPACES_CHAR(lineContent[indexInLine]) && indexInLine <= MAX_LINE_LENGTH; indexInLine++, j++)
			macro[j] = lineContent[indexInLine];
			
		macro[j] = '\0';

		*isInMacro = TRUE;
		macroListAppend(macro);
		return TRUE;
	}

	/* Checks if this line is end of a macro */
	if (strncmp("endm", lineContent+indexInLine, 4) == 0 && *isInMacro) {
		*isInMacro = FALSE;
		indexInLine=0;
		while (indexInLine < MAX_LINE_LENGTH)
		{
			macro[indexInLine] = '\0';
			indexInLine++;
		}
		
		return TRUE;
	}

	/* write regular line to am file */
	if (!*isInMacro)	
		fprintf(amFilePtr, "%s", lineContent);
	/* the line is part of a macro so we added it to macro content */
	else {
		temp = macroLineInList(macro);
		temp->numOfContentLines += 1;
		temp->contentLines = realloc(temp->contentLines, temp->numOfContentLines*sizeof(lineContent));
		lineInMacro = safeMalloc(sizeof(char) * (strlen(lineContent) + 1));
		strcpy(lineInMacro,lineContent);
		temp->contentLines[temp->numOfContentLines-1] = lineInMacro;
		return TRUE;
	}	

    return TRUE;
}


int translateMacros(FILE *assemblyFile, char* fileName)
{
	int macroPassSuccess = TRUE;
	char macro[MAX_LINE_LENGTH];
	int isInMacro = FALSE;
	char lineContent[MAX_LINE_LENGTH + 2];
	int line = 1;

	FILE *assemblyFilePtr;
	FILE *amFilePtr;

	int hasFilesOpend = openFileSafe(&amFilePtr, fileName, ".am", "w") && openFileSafe(&assemblyFilePtr, fileName, ".as", "r");
	if(!hasFilesOpend) {
		return FALSE;
	}

	/* run the macroProcessLine function on every line in .as file */
	for (; fgets(lineContent, MAX_LINE_LENGTH + 2, assemblyFilePtr) != NULL; line++)
	{
		/* run processMacroLine function */ 
		if (!processMacroLine(line, lineContent, &isInMacro, macro, fileName, amFilePtr)) {
			macroPassSuccess = FALSE;
		}
	}
	
	/* free dynamic allocated memory used for macro and close am and assembly files */
	fclose(amFilePtr);
	fclose(assemblyFilePtr);
	freeMacroList();
	return macroPassSuccess;
}
