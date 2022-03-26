#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firstPass.h"
#include "utils.h"

/* recursive function that handles string line */
int handleString(int lineIndex, char *LineContent, int i, int* DC) {
	dataWord *dataToAdd;
	dataToAdd = (dataWord *) safeMalloc(sizeof(dataWord));
	

	if (LineContent[i] == '"') { /* reached the end of the string */
		i++;
		INCREASE_I_UNTILL_NEXT_CHAR(LineContent, i);
		if (LineContent[i] != '\0' && LineContent[i] != '\n' && LineContent[i] != EOF) { /* if there is extra text after quotes */
			printErrorMessage(lineIndex, "Invalid string provided");
			return FALSE;
		}
		dataToAdd->data = '\0';
		machineDataSection[*DC] = *dataToAdd; /* add '\0' to the data section array because all of the chars already added */
		free(dataToAdd);
		(*DC)++;
		return TRUE;	
	}

	dataToAdd->data = LineContent[i];
	machineDataSection[*DC] = *dataToAdd; /* add the char to the data section array */
	free(dataToAdd);
	i++;
	
	(*DC)++;
	return handleString(lineIndex, LineContent, i, DC); /* start function again to find the next char in string */
}

int handleData(int lineIndex, char *lineContent, int i, int *DC, int isReadingFirstParam) {
	int j;
	char dataParam[MAX_LINE_LEN];
	dataWord* dataToAdd;

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent,i);

	if (lineContent[i] == '\0' || lineContent[i] == '\n' || lineContent[i] == EOF) /* if we got to the end of the line/file stop adding data */
		return TRUE;

	if(!isReadingFirstParam) {
		if (lineContent[i] == ',') /* continue after comma */
			i++;

		INCREASE_I_UNTILL_NEXT_CHAR(lineContent,i);
	}

	for (j = 0; lineContent[i] && lineContent[i] != ',' && lineContent[i] != '\n' && lineContent[i] != '\t' && lineContent[i] != ' ' && lineContent[i] != EOF; i++, j++) {
		dataParam[j] = lineContent[i];
	}
	dataParam[j] = '\0'; /* save the number in char array */
	if(isInteger(dataParam)){ /* if string is valida (it's an integer) add it to the data section array */
		dataToAdd = (dataWord *) safeMalloc(sizeof(dataWord));
		dataToAdd->data = atoi(dataParam);
		machineDataSection[*DC] = *dataToAdd;
		free(dataToAdd);
		(*DC)++;
	}
	else {
		if (dataParam[0] == '\0') { /* this happens in case of invalid comma see test2 input and output */
			printErrorMessage(lineIndex, "Invalid comma");
			return FALSE;
		}
		printErrorMessage(lineIndex, "Invalid data provided: \"%s\"", dataParam); /* this happens in case of invalid data see test2 input and output */
		return FALSE;
	}

	return handleData(lineIndex, lineContent, i, DC, FALSE); /* start function again to find the next numbers in data row */

}

int handleCode(int lineIndex, char *lineContent, int i, int* IC) {
	int j, numOfOperands=0;
	char *operandsArray[2];
	assemblyStructure *opcodeData;
	codeWord *firstWord;
	codeWord *secondWord;
	char functionName[MAX_LINE_LEN];

	for (j = 0; lineContent[i] && lineContent[i] != '\n' && lineContent[i] != '\t' && lineContent[i] != ' '  && lineContent[i] != EOF; i++, j++) {
		functionName[j] = lineContent[i];
	}
	functionName[j] = '\0';
	opcodeData = fetchFunctionData(functionName); /* get assembly function data (opcode,funct,operands) */
	if(!opcodeData) { /* in case of function that not exists throw error */
		printErrorMessage(lineIndex, "Invalid assembly function: \"%s\"", functionName);
		return FALSE;
	}

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i); 

	if (!fetchOperands(lineIndex, lineContent, i, operandsArray, &numOfOperands))  {
		return FALSE;
	}

	secondWord = generateSecondCodeWord(lineIndex, lineContent, opcodeData, operandsArray, numOfOperands);
	if (!secondWord) {
		return FALSE;
	}

	firstWord = generateFirstCodeWord(opcodeData);

	machineCodeSection[*IC-IC_INIT_VALUE] = *firstWord;
	machineCodeSection[(*IC+1)-IC_INIT_VALUE] = *secondWord;

	if (numOfOperands == 0) {
		machineCodeSection[*IC-IC_INIT_VALUE] = *firstWord;
		*IC += firstWord->L;
	}
	else {
		machineCodeSection[*IC-IC_INIT_VALUE] = *firstWord;
		machineCodeSection[(*IC+1)-IC_INIT_VALUE] = *secondWord;
		*IC += firstWord->L + secondWord->L;
	}

	return TRUE;
}


int parseLineForFirstPass(int lineIndex, char *lineContent, symbolTable* table, int *IC, int *DC) {
    char externSymbol[MAX_LINE_LEN];
	int dataType;
	int i=0, j;
	char symbolName[MAX_LINE_LEN];
	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i); 

	if (!lineContent[0] || lineContent[0] == '\n' || lineContent[0] == EOF || lineContent[0] == ';')
		return TRUE; 

	if (!fetchSymbol(lineIndex, lineContent, symbolName)) {
		return FALSE;
	}
	if (IS_STRING_EXISTS(symbolName)) {
		if (isSymbolExistsInTable(symbolName, table)) {
			printErrorMessage(lineIndex, "Symbol is already defined: \"%s\"", symbolName);
			return FALSE;
		}
		INCREASE_I_UNTILL_CHAR(lineContent, ':', i);
	}

	if (lineContent[i] == '\n' || !lineContent[i]) {
		return TRUE; 
	}

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i);

	dataType = fetchType(lineContent, &i);
	if (dataType == ERROR) {
		printErrorMessage(lineIndex, "Unable to detect label type");
		return FALSE;
	}

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i);
	if ( dataType == CODE ) {
		if (IS_STRING_EXISTS(symbolName)) {
			symbolTableAppend(symbolName, CODE, table, *IC, *DC);
		}
		if (!handleCode(lineIndex, lineContent, i, IC)) {
			return FALSE;
		}

	}
	else {
		if (dataType == DATA || dataType == STRING) {
			if (IS_STRING_EXISTS(symbolName)){
				symbolTableAppend(symbolName, DATA, table, *IC, *DC);
			}
			if (dataType == DATA) {
				if (!handleData(lineIndex, lineContent, i, DC, TRUE))
					return FALSE;
			}
			else {
				if (!validateString(lineIndex, lineContent, i)) {
					return FALSE;
				}
				if (!handleString(lineIndex, lineContent, i+1, DC))
					return FALSE;
			}	
		}
		else if (dataType == EXTERN) {
			for (j = 0; lineContent[i] && lineContent[i] != '\n' && lineContent[i] != ' ' && lineContent[i] != '\t' && lineContent[i] != EOF; i++, j++) {
				externSymbol[j] = lineContent[i];
			}
			externSymbol[j] = '\0';
			/* If invalid external label name, it's an error */
			if (!isSymbolNameValid(externSymbol, lineIndex)) {
				printErrorMessage(lineIndex, "Invalid symbol for extern type");
				return FALSE;
			}
			if (!isSymbolExistsInTable(externSymbol, table)){
				symbolTableAppend(externSymbol, EXTERN, table, *IC, *DC); /* Extern value is defaulted to 0 */
			}
		}
	}
   	
	return TRUE;
}

int runFirstPass(FILE* fileAfterMacroParsing, symbolTable* table, int* IC, int* DC) {
	int lineIndex;
	char lineContent[MAX_LINE_WITH_LINEDROP_LEN];
	int lineParseSuccess = true;
	for (lineIndex = 0; fgets(lineContent, MAX_LINE_WITH_LINEDROP_LEN, fileAfterMacroParsing); lineIndex++) {
		if (!feof(fileAfterMacroParsing) && strchr(lineContent, '\n') == NULL)
		{
			printErrorMessage(lineIndex, "line exceeds the max line length");
			lineParseSuccess = FALSE;
			/* if the line is too long continue the rest of the chars to get to the new line */ 
			while (fgetc(fileAfterMacroParsing) != '\n');
		} else {
			lineParseSuccess &= parseLineForFirstPass(lineIndex, lineContent, table, IC, DC);
		}
	}
	updateSymbolTableDataTypes(table, *IC);
	return lineParseSuccess;
}
