#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "macroStructs.h"

/* This function handles the macro replacement */
int processMacroLine(char* lineContent, int* isInMacro, char* macroString, char* file, FILE* amFilePtr) {
	int indexInLine=0, indexInWord=0, indexInMacroLine=0;
	char firstWord[MAX_LINE_LENGTH + 2];
	macroLine* currentMacroLine;
	char* lineInMacro = "";
	/* increase index until is not whitespace/tab/newline */
	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
	/* Get the line content after removing spaces and tabs from the beginning until reach whitespace/tab/newline */
	for (; IS_TRUE_CHAR(lineContent[indexInLine]) && indexInLine <= MAX_LINE_LENGTH; indexInLine++, indexInWord++)
		firstWord[indexInWord] = lineContent[indexInLine];
	firstWord[indexInWord] = '\0';

	/* Check if the line contains macro that we already saved */
	if(getMacroLine(firstWord) && !*isInMacro) {
		currentMacroLine = getMacroLine(firstWord);
		/* replace macro name with macro content */
		while (indexInMacroLine < currentMacroLine->numOfContentLines) {
			fprintf(amFilePtr, "%s", currentMacroLine->contentLines[indexInMacroLine++]);
		}
		return TRUE;
	}

	indexInLine = 0;
	indexInWord = 0;
	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine); 
	/* Checks if this line is start of new macro */
    if (strncmp("macro", lineContent+indexInLine, 5) == 0 && !*isInMacro) {
		/* go to the end of the word "macro" */
		INCREASE_INDEX_UNTILL_CHAR(lineContent, 'o', indexInLine);
		INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
		for (; IS_TRUE_CHAR(lineContent[indexInLine]) && indexInLine <= MAX_LINE_LENGTH; indexInLine++, indexInWord++)
			macroString[indexInWord] = lineContent[indexInLine];
			
		macroString[indexInWord] = '\0';

		*isInMacro = TRUE;
		appendToMacroList(macroString);
		return TRUE;
	}

	/* Checks if this line is end of a macro */
	if (strncmp("endm", lineContent+indexInLine, 4) == 0 && *isInMacro) {
		*isInMacro = FALSE;
		indexInLine=0;
		while (indexInLine < MAX_LINE_LENGTH)
		{
			macroString[indexInLine] = '\0';
			indexInLine++;
		}
		
		return TRUE;
	}

	/* the line is part of a macro so we added it to macro content */
	if (*isInMacro)	{
		currentMacroLine = getMacroLine(macroString);
		currentMacroLine->numOfContentLines += 1;
		currentMacroLine->contentLines = realloc(currentMacroLine->contentLines, currentMacroLine->numOfContentLines*sizeof(lineContent));
		lineInMacro = safeMalloc(sizeof(char) * (strlen(lineContent) + 1));
		strcpy(lineInMacro,lineContent);
		currentMacroLine->contentLines[currentMacroLine->numOfContentLines-1] = lineInMacro;
	}
	/* write regular line to the .am file */
	else {
		fprintf(amFilePtr, "%s", lineContent);
	}	

    return TRUE;
}


int translateMacros(FILE* assemblyFile, char* fileName)
{
	int macroPassSuccess = TRUE;
	char macro[MAX_LINE_LENGTH];
	int isInMacro = FALSE;
	char lineContent[MAX_LINE_LENGTH + 2];

	FILE* assemblyFilePtr;
	FILE* amFilePtr;

	int hasFilesOpend = openFileSafe(&amFilePtr, fileName, ".am", "w") && openFileSafe(&assemblyFilePtr, fileName, ".as", "r");
	if(!hasFilesOpend) {
		return FALSE;
	}

	/* run the macroProcessLine function on every line in .as file */
	while (fgets(lineContent, MAX_LINE_LENGTH + 2, assemblyFilePtr))
	{
		/* run processMacroLine function */ 
		if (!processMacroLine(lineContent, &isInMacro, macro, fileName, amFilePtr)) {
			macroPassSuccess = FALSE;
		}
	}
	
	/* free dynamic allocated memory used for macro and close am and assembly files */
	fclose(amFilePtr);
	fclose(assemblyFilePtr);
	freeMacroList();
	return macroPassSuccess;
}
