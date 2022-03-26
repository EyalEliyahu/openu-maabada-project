#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firstPass.h"
#include "utils.h"

int runFirstPass(FILE* fileAfterMacroParsing, symbolTable* table, int* IC, int* DC,
	codeInstruction codeInstructionsList[MAX_MACHINE_CODE_SECTION],
	dataInstruction dataInstructionsList[MAX_MACHINE_DATA_SECTION]
);
int parseLineForFirstPass(
	int lineIndex, char* lineContent, symbolTable* table, int* IC, int* DC,
	codeInstruction codeInstructionsList[MAX_MACHINE_CODE_SECTION],
	dataInstruction dataInstructionsList[MAX_MACHINE_DATA_SECTION]
);
int parseStringEntry(int lineIndex, char* lineContent, int indexInLine, int* DC, dataInstruction dataInstructionsList[MAX_MACHINE_DATA_SECTION]);
int parseDataEntry(int lineIndex, char* lineContent, int indexInLine, int* DC, int isReadingFirstParam, dataInstruction dataInstructionsList[MAX_MACHINE_DATA_SECTION]);
int parseCodeEntry(int lineIndex, char* lineContent, int indexInLine, int* IC, codeInstruction codeInstructionsList[MAX_MACHINE_CODE_SECTION]);

/*handle first pass parsing */
int runFirstPass(
	FILE* fileAfterMacroParsing, symbolTable* table, int* IC, int* DC, 
	codeInstruction codeInstructionsList[MAX_MACHINE_CODE_SECTION],
	dataInstruction dataInstructionsList[MAX_MACHINE_DATA_SECTION]
) {
	int lineIndex;
	char lineContent[MAX_LINE_WITH_LINEDROP_LEN];
	int lineParseSuccess = true;
	for (lineIndex = 0; fgets(lineContent, MAX_LINE_WITH_LINEDROP_LEN, fileAfterMacroParsing); lineIndex++) {
		if (!feof(fileAfterMacroParsing) && strchr(lineContent, '\n') == NULL)
		{
			printLineError(lineIndex, "Max line length exceeded ");
			lineParseSuccess = FALSE;
			/* go to next new line in case of line is over max length */ 
			while (fgetc(fileAfterMacroParsing) != '\n');
		} else {
			lineParseSuccess &= parseLineForFirstPass(lineIndex, lineContent, table, IC, DC, codeInstructionsList, dataInstructionsList);
		}
	}
	updateSymbolTableDataTypes(table, *IC);
	return lineParseSuccess;
}

/* handle line parsing in first pass*/
int parseLineForFirstPass(
	int lineIndex, char* lineContent, symbolTable* table, int* IC, int* DC,
	codeInstruction codeInstructionsList[MAX_MACHINE_CODE_SECTION],
	dataInstruction dataInstructionsList[MAX_MACHINE_DATA_SECTION]
) {
    char externSymbol[MAX_LINE_LENGTH];
	int dataType;
	int indexInLine=0, indexInExternSymbol;
	char symbolName[MAX_LINE_LENGTH];
	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine); 

	if (IS_NULLISH_CHAR(lineContent[indexInLine]) || lineContent[0] == ';')
		return TRUE; 

	if (!getSymbolFromLine(lineIndex, lineContent, symbolName)) {
		return FALSE;
	}
	if (IS_STRING_EXISTS(symbolName)) {
		if (getSymbolItem(symbolName, table)) {
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
		printLineError(lineIndex, "can't recognize label type");
		return FALSE;
	}

	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
	if ( dataType == CODE ) {
		if (IS_STRING_EXISTS(symbolName)) {
			symbolTableAppend(symbolName, CODE, table, *IC, *DC);
		}
		if (!parseCodeEntry(lineIndex, lineContent, indexInLine, IC, codeInstructionsList)) {
			return FALSE;
		}

	}
	else {
		if (dataType == DATA || dataType == STRING) {
			if (IS_STRING_EXISTS(symbolName)){
				symbolTableAppend(symbolName, DATA, table, *IC, *DC);
			}
			if (dataType == DATA) {
				if (!parseDataEntry(lineIndex, lineContent,indexInLine, DC, TRUE, dataInstructionsList))
					return FALSE;
			}
			else {
				if (!validateStringEntry(lineIndex, lineContent, indexInLine)) {
					return FALSE;
				}
				if (!parseStringEntry(lineIndex, lineContent, indexInLine+1, DC, dataInstructionsList))
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
			if (!getSymbolItem(externSymbol, table)){
				symbolTableAppend(externSymbol, EXTERN, table, *IC, *DC); /* 0 is default extern value */
			}
		}
	}
   	
	return TRUE;
}

/* recursive function that handle parsing of string line */
int parseStringEntry(int lineIndex, char* lineContent, int indexInLine, int* DC) {
	dataWord *dataToAdd;
	dataToAdd = (dataWord *) safeMalloc(sizeof(dataWord));
	

	if (lineContent[indexInLine] == '"') { /* go to the end of string */
		indexInLine++;
		INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
		if (!IS_NULLISH_CHAR(lineContent[indexInLine])) { /* check if quotes are the end of line */
			printLineError(lineIndex, "Invalid string provided");
			return FALSE;
		}
		dataToAdd->data = '\0';
		dataInstructionsList[*DC] = *dataToAdd; /* add '\0' to the data section array because all of the chars already added */
		machineDataSection[*DC] = *dataToAdd; /* add '\0' to the data section string because all of the chars already added */
		free(dataToAdd);
		(*DC)++;
		return TRUE;	
	}

	dataToAdd->data = lineContent[indexInLine];
	dataInstructionsList[*DC] = *dataToAdd; /* add the char to the data section array */
	machineDataSection[*DC] = *dataToAdd; /* add the char to the machine data section string */
	free(dataToAdd);
	indexInLine++;
	
	(*DC)++;
	return parseStringEntry(lineIndex, lineContent,indexInLine, DC, dataInstructionsList); /* start function again to find the next char in string */
}

/* recursive function that handle parsing of data line */
int parseDataEntry(int lineIndex, char* lineContent, int indexInLine, int* DC, int isReadingFirstParam, dataInstruction dataInstructionsList[MAX_MACHINE_DATA_SECTION]) {
	return parseStringEntry(lineIndex, lineContent,indexInLine, DC); /* looking for the next char in string */
}

int parseDataEntry(int lineIndex, char* lineContent, int indexInLine, int* DC, int isReadingFirstParam) {
	int indexInDataParam;
	char dataParam[MAX_LINE_LENGTH];
	dataInstruction* dataToAdd;

	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);

	if (IS_NULLISH_CHAR(lineContent[indexInLine])) /* we stop to add data in case of reach nullish char */
		return TRUE;

	if(!isReadingFirstParam) {
		if (lineContent[indexInLine] == ',') /* continue after comma */
			indexInLine++;

		INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
	}

	for (indexInDataParam = 0; IS_TRUE_CHAR(lineContent[indexInLine]) && lineContent[indexInLine] != ','; indexInLine++, indexInDataParam++) {
		dataParam[indexInDataParam] = lineContent[indexInLine];
	}

	dataParam[indexInDataParam] = '\0'; 
	if(isStringInteger(dataParam)){ /* add string data section array if valid */
		dataToAdd = (dataWord *) safeMalloc(sizeof(dataWord));
		dataToAdd->data = atoi(dataParam);
		dataInstructionsList[*DC] = *dataToAdd;
		free(dataToAdd);
		(*DC)++;
	}
	else {
		if (IS_NULLISH_CHAR(dataParam[0])) { /* case of invalid comma */
			printLineError(lineIndex, "Invalid comma");
			return FALSE;
		}
		printLineError(lineIndex, "Invalid data: \"%s\"", dataParam); /* case of invalid data */
		return FALSE;
	}

	return parseDataEntry(lineIndex, lineContent,indexInLine, DC, FALSE, dataInstructionsList); /* start function again to find the next numbers in data row */
}
/* function that handle parsing of code line */
int parseCodeEntry(int lineIndex, char* lineContent, int indexInLine, int* IC, codeInstruction codeInstructionsList[MAX_MACHINE_CODE_SECTION]) {
	int indexInFunctionName, numOfOperands=0;
	char* operandsArray[2];
	optCodeData *opcodeData;
	codeInstruction *firstWord;
	codeInstruction *secondWord;
	char functionName[MAX_LINE_LENGTH];

	for (indexInFunctionName = 0; IS_TRUE_CHAR(lineContent[indexInLine]); indexInLine++, indexInFunctionName++) {
		functionName[indexInFunctionName] = lineContent[indexInLine];
	}
	functionName[indexInFunctionName] = '\0';
	opcodeData = fetchFunctionData(functionName); /* get assembly function data (opcode,funct,operands) */
	if(!opcodeData) { /*  throw error if function not exists */
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

	codeInstructionsList[*IC-IC_INIT_VALUE] = *firstWord;
	codeInstructionsList[(*IC+1)-IC_INIT_VALUE] = *secondWord;

	if (numOfOperands == 0) {
		codeInstructionsList[*IC-IC_INIT_VALUE] = *firstWord;
		*IC += firstWord->L;
	}
	else {
		codeInstructionsList[*IC-IC_INIT_VALUE] = *firstWord;
		codeInstructionsList[(*IC+1)-IC_INIT_VALUE] = *secondWord;
		*IC += firstWord->L + secondWord->L;
	}

	return TRUE;
}
