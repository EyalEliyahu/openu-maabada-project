#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firstPass.h"
#include "utils.h"

/* recursive function that handles string line */
int parseStringEntry(int lineIndex, char *lineContent, int indexInLine, int* DC) {
	dataWord *dataToAdd;
	dataToAdd = (dataWord *) safeMalloc(sizeof(dataWord));
	

	if (lineContent[indexInLine] == '"') { /* reached the end of the string */
		indexInLine++;
		INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
		if (!IS_NULLISH_CHAR(lineContent[indexInLine])) { /* if there is extra text after quotes */
			printLineError(lineIndex, "Invalid string provided");
			return FALSE;
		}
		dataToAdd->data = '\0';
		machineDataSection[*DC] = *dataToAdd; /* add '\0' to the data section array because all of the chars already added */
		free(dataToAdd);
		(*DC)++;
		return TRUE;	
	}

	dataToAdd->data = lineContent[indexInLine];
	machineDataSection[*DC] = *dataToAdd; /* add the char to the data section array */
	free(dataToAdd);
	indexInLine++;
	
	(*DC)++;
	return parseStringEntry(lineIndex, lineContent,indexInLine, DC); /* start function again to find the next char in string */
}

int parseDataEntry(int lineIndex, char *lineContent, int indexInLine, int *DC, int isReadingFirstParam) {
	int indexInDataParam;
	char dataParam[MAX_LINE_LEN];
	dataWord* dataToAdd;

	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);

	if (IS_NULLISH_CHAR(lineContent[indexInLine])) /* if we got to the end of the line/file stop adding data */
		return TRUE;

	if(!isReadingFirstParam) {
		if (lineContent[indexInLine] == ',') /* continue after comma */
			indexInLine++;

		INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
	}

	for (indexInDataParam = 0; IS_TRUE_CHAR(lineContent[indexInLine]) && lineContent[indexInLine] != ','; indexInLine++, indexInDataParam++) {
		dataParam[indexInDataParam] = lineContent[indexInLine];
	}
	dataParam[indexInDataParam] = '\0'; /* save the number in char array */
	if(isStringInteger(dataParam)){ /* if string is valida (it's an integer) add it to the data section array */
		dataToAdd = (dataWord *) safeMalloc(sizeof(dataWord));
		dataToAdd->data = atoi(dataParam);
		machineDataSection[*DC] = *dataToAdd;
		free(dataToAdd);
		(*DC)++;
	}
	else {
		if (IS_NULLISH_CHAR(dataParam[0])) { /* this happens in case of invalid comma see test2 input and output */
			printLineError(lineIndex, "Invalid comma");
			return FALSE;
		}
		printLineError(lineIndex, "Invalid data provided: \"%s\"", dataParam); /* this happens in case of invalid data see test2 input and output */
		return FALSE;
	}

	return parseDataEntry(lineIndex, lineContent,indexInLine, DC, FALSE); /* start function again to find the next numbers in data row */

}

int parseCodeEntry(int lineIndex, char *lineContent, int indexInLine, int* IC) {
	int indexInFunctionName, numOfOperands=0;
	char *operandsArray[2];
	assemblyStructure *opcodeData;
	codeWord *firstWord;
	codeWord *secondWord;
	char functionName[MAX_LINE_LEN];

	for (indexInFunctionName = 0; IS_TRUE_CHAR(lineContent[indexInLine]); indexInLine++, indexInFunctionName++) {
		functionName[indexInFunctionName] = lineContent[indexInLine];
	}
	functionName[indexInFunctionName] = '\0';
	opcodeData = fetchFunctionData(functionName); /* get assembly function data (opcode,funct,operands) */
	if(!opcodeData) { /* in case of function that not exists throw error */
		printLineError(lineIndex, "Invalid assembly function: \"%s\"", functionName);
		return FALSE;
	}

	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine); 

	if (!fetchOperands(lineIndex, lineContent,indexInLine, operandsArray, &numOfOperands))  {
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
	int indexInLine=0, indexInExternSymbol;
	char symbolName[MAX_LINE_LEN];
	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine); 

	if (IS_NULLISH_CHAR(lineContent[indexInLine]) || lineContent[0] == ';')
		return TRUE; 

	if (!getSymbolFromLine(lineIndex, lineContent, symbolName)) {
		return FALSE;
	}
	if (IS_STRING_EXISTS(symbolName)) {
		if (isSymbolExistsInTable(symbolName, table)) {
			printLineError(lineIndex, "Symbol is already defined: \"%s\"", symbolName);
			return FALSE;
		}
		INCREASE_INDEX_UNTILL_CHAR(lineContent, ':', indexInLine);
	}

	if (IS_NULLISH_CHAR(lineContent[indexInLine])) {
		return TRUE; 
	}

	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);

	dataType = fetchType(lineContent, &indexInLine);
	if (dataType == ERROR) {
		printLineError(lineIndex, "Unable to detect label type");
		return FALSE;
	}

	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
	if ( dataType == CODE ) {
		if (IS_STRING_EXISTS(symbolName)) {
			symbolTableAppend(symbolName, CODE, table, *IC, *DC);
		}
		if (!parseCodeEntry(lineIndex, lineContent, indexInLine, IC)) {
			return FALSE;
		}

	}
	else {
		if (dataType == DATA || dataType == STRING) {
			if (IS_STRING_EXISTS(symbolName)){
				symbolTableAppend(symbolName, DATA, table, *IC, *DC);
			}
			if (dataType == DATA) {
				if (!parseDataEntry(lineIndex, lineContent,indexInLine, DC, TRUE))
					return FALSE;
			}
			else {
				if (!validateStringEntry(lineIndex, lineContent, indexInLine)) {
					return FALSE;
				}
				if (!parseStringEntry(lineIndex, lineContent, indexInLine+1, DC))
					return FALSE;
			}	
		}
		else if (dataType == EXTERN) {
			for (indexInExternSymbol = 0; !IS_NULLISH_CHAR(lineContent[indexInLine]) && !IS_WHITESPACES_CHAR(lineContent[indexInLine]); indexInLine++, indexInExternSymbol++) {
				externSymbol[indexInExternSymbol] = lineContent[indexInLine];
			}
			externSymbol[indexInExternSymbol] = '\0';
			/* If invalid external label name, it's an error */
			if (!isSymbolNameValid(externSymbol, lineIndex)) {
				printLineError(lineIndex, "Invalid symbol for extern type");
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
			printLineError(lineIndex, "line exceeds the max line length");
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
