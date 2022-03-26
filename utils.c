#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>
#include "utils.h"

void printLineError(int lineIndex, char* message, ...)
{
	va_list messageArgs;
	printf("ASSEMBLER ERROR [line %d]: ", lineIndex + 1);
	va_start(messageArgs, message);
	vprintf(message, messageArgs);
	va_end(messageArgs);
	printf("\n");
}

int isStringInteger(char* string)
{
	int i = 0;
	if (string[0] == '-' || string[0] == '+')
		i++; /* if string starts + or - */
	for (; string[i]; i++)
	{ /* validate all of the rest are digits */
		if (!isdigit(string[i]))
		{
			return FALSE;
		}
	}
	return i > 0; /* i will be 0 if string is empty and it's not a number */
}

void *safeMalloc(long size)
{
	void *ptr = malloc(size);
	if (ptr == NULL)
	{
		printf("\nFatal Error: Failed allocating memory.");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

char* stringsConcat(char* string1, char* string2)
{
	char* concattedString = (char* )safeMalloc(strlen(string1) + strlen(string2) + 1);
	strcpy(concattedString, string1);
	strcat(concattedString, string2);
	return concattedString;
}

int openFileSafe(FILE* *fileStream, char* fileName, char* fileExt, char* openMethod)
{
	char* fileNameWithExt = stringsConcat(fileName, fileExt);
	*fileStream = fopen(fileNameWithExt, openMethod);
	if (*fileStream == NULL)
	{
		printf("Error: Unable to read file: \"%s\". skipping it.\n", fileNameWithExt);
		free(fileNameWithExt);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

int isSymbolWithValidIndex(char* operandString, int lineIndex)
{
	char symbolPart[MAX_LINE_LENGTH];
	int indexInSymbolPart;

	for (indexInSymbolPart = 0; operandString[indexInSymbolPart] && operandString[indexInSymbolPart] != '[' && operandString[indexInSymbolPart] != '\0'; indexInSymbolPart++)
	{
		symbolPart[indexInSymbolPart] = operandString[indexInSymbolPart];
	}

	symbolPart[indexInSymbolPart] = '\0';
	if (!isSymbolNameValid(symbolPart, lineIndex))
		return FALSE;

	if (
		operandString[indexInSymbolPart] == '[' &&
		operandString[indexInSymbolPart + 1] == 'r' &&
		atoi(&operandString[indexInSymbolPart + 2]) <= 9 &&
		operandString[indexInSymbolPart + 3] == ']' &&
		operandString[indexInSymbolPart + 4] == '\0') /* SYMBOL[rx] */
		return TRUE;

	if (
		operandString[indexInSymbolPart] == '[' &&
		operandString[indexInSymbolPart + 1] == 'r' &&
		atoi(&operandString[indexInSymbolPart + 2]) >= 10 &&
		atoi(&operandString[indexInSymbolPart + 2]) <= 15 &&
		operandString[indexInSymbolPart + 4] == ']' &&
		operandString[indexInSymbolPart + 5] == '\0') /* SYMBOL[r1x] */
		return TRUE;

	return FALSE;
}

int getSymbolFromLine(int lineIndex, char* lineContent, char* symbolDest)
{
	int indexAtSymbol = 0;
	int indexAtLine = 0;

	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexAtLine);

	for (; !IS_NULLISH_CHAR(lineContent[indexAtLine]) && lineContent[indexAtLine] != ':' && indexAtLine <= MAX_LINE_LENGTH; indexAtLine++, indexAtSymbol++)
	{
		symbolDest[indexAtSymbol] = lineContent[indexAtLine];
	}
	symbolDest[indexAtSymbol] = '\0';

	if (lineContent[indexAtSymbol] == ':')
	{
		if (!isSymbolNameValid(symbolDest, lineIndex))
		{
			printLineError(lineIndex, "invalid symbol name: \"%s\"", symbolDest);
			symbolDest[0] = '\0';
			return FALSE;
		}
		return TRUE;
	}
	symbolDest[0] = '\0';
	return TRUE;
}

int fetchOperands(int lineIndex, char* lineContent, int indexAtLine, char* *operandsArray, int* numOfOperands)
{
	int indexInOperandStr;
	operandsArray[0] = operandsArray[1] = NULL;

	if (lineContent[indexAtLine] == ',')
	{
		printLineError(lineIndex, "Unexpected comma");
		return FALSE;
	}

	for (*numOfOperands = 0; !IS_NULLISH_CHAR(lineContent[indexAtLine]);)
	{
		if (*numOfOperands == 2)
		{
			printLineError(lineIndex, "Too many operands for this assembly command");
			free(operandsArray[0]);
			free(operandsArray[1]);
			return FALSE;
		}

		operandsArray[*numOfOperands] = safeMalloc(MAX_LINE_LENGTH);
		for (indexInOperandStr = 0; IS_TRUE_CHAR(lineContent[indexAtLine]) && lineContent[indexAtLine] != ','; indexAtLine++, indexInOperandStr++)
		{
			operandsArray[*numOfOperands][indexInOperandStr] = lineContent[indexAtLine];
		}
		operandsArray[*numOfOperands][indexInOperandStr] = '\0';
		(*numOfOperands)++;
		INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexAtLine);

		if (IS_NULLISH_CHAR(lineContent[indexAtLine]))
			break;
		else if (lineContent[indexAtLine] != ',')
		{
			printLineError(lineIndex, "There needs to be a comma between operands");
			free(operandsArray[0]);
			if (*numOfOperands > 1)
				free(operandsArray[1]);
				
			return FALSE;
		}
		indexAtLine++;
		INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexAtLine);
		if (IS_NULLISH_CHAR(lineContent[indexAtLine]))
			printLineError(lineIndex, "There needs to be an operand after comma");
		else if (lineContent[indexAtLine] == ',')
			printLineError(lineIndex, "Multiple consecutive commas.");
		else
			continue;
		{
			free(operandsArray[0]);
			if (*numOfOperands > 1)
			{
				free(operandsArray[1]);
			}
			return FALSE;
		}
	}
	return TRUE;
}

int fetchType(char* lineContent, int* indexInLine) {
	char typeString[MAX_LINE_LENGTH];
	int indexInThypeString = 0;

	if (lineContent[*indexInLine] != '.'){
		return CODE;
	}

	for (; IS_TRUE_CHAR(lineContent[*indexInLine]); (*indexInLine)++, indexInThypeString++)
	{
		typeString[indexInThypeString] = lineContent[*indexInLine];
	}
	typeString[indexInThypeString] = '\0';
	if (IS_STR_EQL(typeString, ".data"))
	{
		return DATA;
	}
	if (IS_STR_EQL(typeString, ".string"))
	{
		return STRING;
	}
	if (IS_STR_EQL(typeString, ".entry"))
	{
		return ENTRY;
	}
	if (IS_STR_EQL(typeString, ".extern"))
	{
		return EXTERN;
	}
	return ERROR;
}

int fetchAddressType(char* operand, int line) {
	if (operand[0] == '\0')
		return NO_ADDRESS;
	if (operand[0] == 'r' && isdigit(operand[1]) && operand[2] == '\0')
		return REGISTER;
	if (operand[0] == 'r' && atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0')
		return REGISTER;
	if (operand[0] == '#' && isStringInteger(operand + 1))
		return IMMEDIATE;
	if (operand[0] && isSymbolWithValidIndex(operand, line))
		return INDEX;
	if (isSymbolNameValid(operand, line))
		return DIRECT;
	return NO_ADDRESS;
}

unsigned int fetchRegister(char* operand) {
	unsigned int reg;
	int index = 0, j;
	char temp[MAX_LINE_LENGTH];

	if (operand[0] == 'r' && isdigit(operand[1]) && operand[2] == '\0')
	{
		reg = atoi(&operand[1]);
		if (reg >= 0 && reg <= 9)
			return reg;
	}

	if (operand[0] == 'r' && atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0')
	{
		reg = atoi(&operand[1]);
		return reg;
	}

	INCREASE_INDEX_UNTILL_CHAR(operand, '[', index);
	index++;
	for (j = 0; operand[index] && operand[index] != ']'; index++, j++)
	{
		temp[j] = operand[index];
	}
	temp[j] = '\0';

	return atoi(temp);
}

codeWord *generateFirstCodeWord(optCodeData *opcodeData) {
	codeWord *resWord;
	resWord = (codeWord *)safeMalloc(sizeof(codeWord));
	resWord->ARE = 4;
	resWord->L = 1;
	resWord->sourceAddress = resWord->sourceRegister = resWord->destinationAddress = resWord->destinationRegister = resWord->opcode = resWord->funct = 0;

	resWord->opcode = opcodeData->opcode;
	return resWord;
}

codeWord *generateSecondCodeWord(int lineIndex, char* lineContent, optCodeData *opcodeData, char* *operandsArray, int numOfOperands) {
	codeWord *resWord;
	int address1 = NO_ADDRESS;
	int address2 = NO_ADDRESS;

	resWord = (codeWord *)safeMalloc(sizeof(codeWord));
	resWord->sourceAddress = resWord->sourceRegister = resWord->destinationAddress = resWord->destinationRegister = resWord->opcode = 0;
	resWord->L = 1;
	resWord->lineIndex = lineIndex;
	resWord->ARE = 4;
	resWord->funct = opcodeData->funct;
	if (numOfOperands > 0)
	{
		address1 = fetchAddressType(operandsArray[0], lineIndex);
	}
	if (numOfOperands > 1)
	{
		address2 = fetchAddressType(operandsArray[1], lineIndex);
	}

	/* validate the operands against the given function */
	if (!validateOperands(lineIndex, address1, address2, numOfOperands, opcodeData))
	{
		return NULL;
	}

	if (opcodeData->numOfOperandsPerFunction == 2)
	{
		resWord->firstOperand = operandsArray[0];
		resWord->secondOperand = operandsArray[1];
		resWord->sourceAddress = address1;
		resWord->destinationAddress = address2;
		if (address1 == REGISTER)
		{
			resWord->sourceRegister = fetchRegister(operandsArray[0]);
		}
		if (address2 == REGISTER)
		{
			resWord->destinationRegister = fetchRegister(operandsArray[1]);
		}
		if (address1 == IMMEDIATE)
		{
			resWord->L += 1;
		}
		if (address2 == IMMEDIATE)
		{
			resWord->L += 1;
		}
		if (address1 == DIRECT)
		{
			resWord->L += 2;
		}
		if (address1 == INDEX)
		{
			resWord->L += 2;
			resWord->sourceRegister = fetchRegister(operandsArray[0]);
		}
		if (address2 == DIRECT)
		{
			resWord->L += 2;
		}
		if (address2 == INDEX)
		{
			resWord->L += 2;
			resWord->destinationRegister = fetchRegister(operandsArray[1]);
		}
	}
	else if (opcodeData->numOfOperandsPerFunction == 1)
	{
		resWord->firstOperand = operandsArray[0];
		resWord->destinationAddress = address1;
		if (address1 == REGISTER)
		{
			resWord->destinationRegister = fetchRegister(operandsArray[0]);
		}
		if (address1 == IMMEDIATE)
		{
			resWord->L += 1;
		}
		if (address1 == DIRECT)
		{
			resWord->L += 2;
		}
		if (address1 == INDEX)
		{
			resWord->L += 2;
			resWord->destinationRegister = fetchRegister(operandsArray[0]);
		}
	}

	return resWord;
}

int calcIcBase(int ic) {
	return ic - (ic % 16);
}

int calcIcOffset(int ic) {
	return ic % 16;
}

int validateStringEntry(int lineIndex, char* lineContent, int indexAtLine ){

	if (lineContent[indexAtLine] != '"')
	{
		printLineError(lineIndex, "String type should start with quotes");
		return FALSE;
	}
	indexAtLine++;

	if (lineContent[indexAtLine] == '"')
	{
		printLineError(lineIndex, "String should not be empty!");
		return FALSE;
	}

	while (lineContent[indexAtLine] != '"' && !IS_NULLISH_CHAR(lineContent[indexAtLine]))
		indexAtLine++;

	if (lineContent[indexAtLine] != '"') {
		printLineError(lineIndex, "String type should end with quotes");
		return FALSE;

	}

	return TRUE;
}

int isAlphanumericStr(char* string) {
	int i;
	/*check for every char in string if it is non alphanumeric char if it is function returns TRUE*/
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])) return FALSE;
	}
	return TRUE;
}
