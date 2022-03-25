#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firstPhase.h"

int handleData(int lineIndex, char *lineContent, int i, int *DC) {
	int j;
	char dataParam[MAX_LINE_LEN];
	dataWord *dataToAdd;

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent,i);

	if (lineContent[i] == '\0' || lineContent[i] == '\n')
		return TRUE;

	if (lineContent[i] == ',')
		i++;

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent,i);

	for (j = 0; lineContent[i] && lineContent[i] != ',' && lineContent[i] != '\n' && lineContent[i] != '\t' && lineContent[i] != ' ' && lineContent[i] != EOF; i++, j++) {
		dataParam[j] = lineContent[i];
	}
	dataParam[j] = '\0';
	
	if(atoi(dataParam)){
		dataToAdd = (dataWord *) malloc(sizeof(dataWord));
		dataToAdd->data = atoi(dataParam);
		machineDataSection[*DC] = *dataToAdd;
		free(dataToAdd);
		(*DC)++;
	}
	else {
		printErrorMessage(lineIndex, "Invalid data provided");
		return FALSE;
	}

	return handleData(lineIndex,lineContent,i, DC);

}

int handleString(int lineIndex, char* lineContent, int i, int* DC) {
	dataWord *dataToAdd;
	dataToAdd = (dataWord *) malloc(sizeof(dataWord));
	

	if (lineContent[i] == '"') {
		dataToAdd->data = '\0';
		machineDataSection[*DC] = *dataToAdd;
		free(dataToAdd);
		(*DC)++;
		return TRUE;	
	}

	dataToAdd->data = lineContent[i];
	machineDataSection[*DC] = *dataToAdd;
	free(dataToAdd);
	i++;
	
	(*DC)++;
	return handleString(lineIndex,lineContent,i, DC);
}

int handleCode(int lineIndex, char *lineContent, int i, int* IC) {
	int j, numOfOperands=0;
	char *operandsArray[2];
	assemblyStructure *opcodeData;
	codeWord *firstWord;
	codeWord *secondWord;
	char functionName[MAX_LINE_LEN];

	for (j = 0; lineContent[i] && lineContent[i] != '\n' && lineContent[i] != '\t' && lineContent[i] != ' ' && lineContent[i] != EOF; i++, j++) {
		functionName[j] = lineContent[i];
	}
	functionName[j] = '\0';
	opcodeData = fetchFunctionData(functionName);

	if(!opcodeData) {
		printErrorMessage(lineIndex, "could not find this assembly command");
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


int parseLineForFirstPhase(int lineIndex, char *lineContent, symbolTable* table, int *IC, int *DC) {
    char externSymbol[MAX_LINE_LEN];
	int dataType;
	int i=0, j;
	char symbolName[MAX_LINE_LEN];
	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i); 
	if (!lineContent[i] || lineContent[i] == '\n' || lineContent[i] == EOF || lineContent[i] == ';')
		return TRUE; 

	if (!fetchSymbol(lineIndex, lineContent, symbolName)) {
		return FALSE;
	}

	if (IS_STRING_EXISTS(symbolName)) {
		if (isSymbolExistsInTable(symbolName, table)) {
			printErrorMessage(lineIndex, "Symbol is already defined.");
			return FALSE;
		}
		INCREASE_I_UNTILL_CHAR(lineContent, ':', i);
	}

	if (lineContent[i] == '\n') {
		return TRUE; 
	}

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i);
	dataType = fetchType(lineContent, &i);
	if (dataType == ERROR) {
		return FALSE;
	}

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i);
	if ( dataType == CODE ) {
		if (IS_STRING_EXISTS(symbolName)) {
			symbolTableAppend(symbolName, CODE, table, *IC, *DC);
		}
		if (!handleCode(lineIndex, lineContent, i, IC))
			return FALSE;

	}
	else {
		if (dataType == DATA || dataType == STRING) {
			if (IS_STRING_EXISTS(symbolName)){
				symbolTableAppend(symbolName, DATA, table, *IC, *DC);
			}
			if (dataType == DATA) {
				if (!handleData(lineIndex, lineContent, i, DC))
					return FALSE;
			}
			else {
				if (!validateString(lineIndex, lineContent, i))
					return FALSE;
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

int runFirstPhase(FILE* fileAfterMacroParsing, symbolTable* table, int* IC, int* DC) {
	int lineIndex;
	char lineContent[MAX_LINE_WITH_LINEDROP_LEN];
	for (lineIndex = 0; fgets(lineContent, MAX_LINE_WITH_LINEDROP_LEN, fileAfterMacroParsing); lineIndex++) {
		int lineParseSuccess = parseLineForFirstPhase(lineIndex, lineContent, table, IC, DC);
		if(!lineParseSuccess) {
			return FALSE;
		}
	}
	updateSymbolTableDataTypes(table, *IC);
	return TRUE;
}
