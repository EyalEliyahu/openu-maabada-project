#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "macroStructs.h"

/* This function handles the macro replacement */
int processMacroLine(int line, char *lineContent, int *inMacro, char *macro, char *file, FILE *amFilePtr) {
	int i=0, j=0, k=0;
	char firstWord[MAX_LINE_LENGTH + 2];
	macroLine *temp;
	char* lineInMacro = "";
	/* look for the next char that is not whitespace/tab/newline */
	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i);
	/* Get the line content after removing spaces and tabs from the beginning until reach whitespace/tab/newline */
	for (; lineContent[i] && lineContent[i] != '\n' && lineContent[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
		firstWord[j] = lineContent[i];
	}
	firstWord[j] = '\0';

	/* Check if the line contains macro that we already saved */
	if(macroExistsInList(firstWord) && !*inMacro) {
		temp = macroLineInList(firstWord);
		/* replace macro name with macro content */
		while (k < temp->numOfContentLines)
		{
			fprintf(amFilePtr, "%s", temp->contentLines[k++]);
		}
		return TRUE;
	}

	i = 0;
	j = 0;
	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i); 
	/* Checks if this line is start of new macro */
    if (strncmp("macro", lineContent+i, 5) == 0 && !*inMacro) {
		/* go to the end of the word: macro */
		INCREASE_I_UNTILL_CHAR(lineContent, 'o', i);
		INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i);
		for (; lineContent[i] && lineContent[i] != '\n' && lineContent[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
			macro[j] = lineContent[i];
		}
		macro[j] = '\0';

		*inMacro = TRUE;
		macroListAppend(macro);
		return TRUE;
	}

	/* Checks if this line is end of a macro */
	if (strncmp("endm", lineContent+i, 4) == 0 && *inMacro) {
		*inMacro = FALSE;
		i=0;
		while (i < MAX_LINE_LENGTH)
		{
			macro[i] = '\0';
			++i;
		}
		
		return TRUE;
	}

	/* write regular line to am file */
	if (!*inMacro)	
		fprintf(amFilePtr, "%s", lineContent);
	/* the line is part of a macro so we added it to macro content */
	else {
		temp = macroLineInList(macro);
		temp->numOfContentLines += 1;
		temp->contentLines = realloc(temp->contentLines, temp->numOfContentLines*sizeof(lineContent));
		lineInMacro = malloc(sizeof(char) * (strlen(lineContent) + 1));
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
	int inMacro = FALSE;
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
		if (!processMacroLine(line, lineContent, &inMacro, macro, fileName, amFilePtr)) {
			macroPassSuccess = FALSE;
		}
	}
	
	/* free dynamic allocated memory used for macro and close am and assembly files */
	fclose(amFilePtr);
	fclose(assemblyFilePtr);
	freeMacroList();
	return macroPassSuccess;
}
