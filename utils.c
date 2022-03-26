#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>
#include "utils.h"

void *safeMalloc(long size) {
	void *ptr = malloc(size);
	if (ptr == NULL) {
		printf("\nFatal Error: Failed allocating memory.");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

char* stringsConcat(char *string1, char* string2) {
    char* concattedString = (char*) safeMalloc(strlen(string1) + strlen(string2) + 1);
	strcpy(concattedString, string1);
	strcat(concattedString, string2);
	return concattedString;
}

int openFileSafe(FILE** fileStream, char* fileName, char* fileExt, char* openMethod) {
	char* fileNameWithExt = stringsConcat(fileName, fileExt);
	*fileStream = fopen(fileNameWithExt, openMethod);
	if(*fileStream == NULL) {
		printf("Error: Unable to read file: \"%s\". skipping it.\n", fileNameWithExt);
		free(fileNameWithExt);
		return FALSE;
	} else {
		return TRUE;
	}
}

void printErrorMessage(int line, char* message, ...) {
	va_list args; 

	printf("ASSEMBLER ERROR [line %d]: ", line+1);

	va_start(args, message);
	vprintf(message, args);
	va_end(args);
	printf("\n");
}

int isInteger(char *string) {
	int i = 0;
	if (string[0] == '-' || string[0] == '+') string++; /* if string starts + or - */
	for (; string[i]; i++) { /* validate all of the rest are digits */
		if (!isdigit(string[i])) {
			return FALSE;
		}
	}
	return i > 0; /* i will be 0 if string is empty and it's not a number */ 
}

int isIndex(char* operand, int line) {
	char symbol[MAX_LINE_LENGTH];
	int j;

	for (j = 0; operand[j] && operand[j] != '[' && operand[j] != '\0'; j++) {
		symbol[j] = operand[j];
	}

    symbol[j] = '\0';
	if (!isSymbolNameValid(symbol, line))
		return FALSE;

	if (operand[j] == '[' && operand[j+1] == 'r' && atoi(&operand[j+2]) <= 9 && operand[j+3] == ']' && operand[j+4] == '\0') /* SYMBOL[rx] */
		return TRUE;

	if (operand[j] == '[' && operand[j+1] == 'r' && atoi(&operand[j+2]) >= 10 && atoi(&operand[j+2]) <= 15 && operand[j+4] == ']' && operand[j+5] == '\0') /* SYMBOL[r1x] */
		return TRUE;	

	return FALSE;
}


int fetchSymbol(int line, char* lineContent, char *symbolDest) {
	int j, i;
	i = j = 0;

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i);

	for (; lineContent[i] && lineContent[i] != ':' && lineContent[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
		symbolDest[j] = lineContent[i];
	}
	symbolDest[j] = '\0';

	if (lineContent[i] == ':') {
		if (!isSymbolNameValid(symbolDest, line)) {
			printErrorMessage(line, "invalid symbol name: \"%s\"" , symbolDest);
			symbolDest[0] = '\0';
			return FALSE; 
		}
		return TRUE;
	}
	symbolDest[0] = '\0';
	return TRUE;
}

int fetchOperands(int line, char* lineContent, int i, char **operandsArray, int *numOfOperands) {
	int j;
	operandsArray[0] = operandsArray[1] = NULL;

	if (lineContent[i] == ',') {
		printErrorMessage(line, "Unexpected comma");
		return FALSE;
	}


	for (*numOfOperands = 0; lineContent[i] != EOF && lineContent[i] != '\n' && lineContent[i];) {
		if (*numOfOperands == 2) {
			printErrorMessage(line, "Too many operands for this assembly command");
			free(operandsArray[0]);
			free(operandsArray[1]);
			return FALSE; 
		}

		operandsArray[*numOfOperands] = safeMalloc(MAX_LINE_LENGTH);
		for (j = 0; lineContent[i] && lineContent[i] != '\t' && lineContent[i] != ' ' && lineContent[i] != '\n' && lineContent[i] != EOF &&
		            lineContent[i] != ','; i++, j++) {
			operandsArray[*numOfOperands][j] = lineContent[i];
		}
		operandsArray[*numOfOperands][j] = '\0';
		(*numOfOperands)++;
		INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i);

		if (lineContent[i] == '\n' || lineContent[i] == EOF || !lineContent[i]) break;
		else if (lineContent[i] != ',') {
			printErrorMessage(line, "There needs to be a comma between operands");
			free(operandsArray[0]);
			if (*numOfOperands > 1) {
				free(operandsArray[1]);
			}
			return FALSE;
		}
		i++;
		INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i);
		if (lineContent[i] == '\n' || lineContent[i] == EOF || !lineContent[i])
			printErrorMessage(line, "There needs to be an operand after comma");
		else if (lineContent[i] == ',') 
			printErrorMessage(line, "Multiple consecutive commas.");
		else 
			continue;
		{
			free(operandsArray[0]);
			if (*numOfOperands > 1) {
				free(operandsArray[1]);
			}
			return FALSE;
		}
	}
	return TRUE;
}

int fetchType(char *lineContent, int *i) {

	char temp[MAX_LINE_LENGTH];
	int j;

	if (lineContent[*i] != '.') {
        return CODE;
    }

	for (j = 0; lineContent[*i] && lineContent[*i] != '\t' && lineContent[*i] != ' '; (*i)++, j++) {
		temp[j] = lineContent[*i];
	}
    temp[j] = '\0';
	if (strcmp(temp, ".data") == 0) {
        return DATA;
    }
    if (strcmp(temp, ".string") == 0) {
        return STRING;
    }
    if (strcmp(temp, ".entry") == 0) {
        return ENTRY;
    }
    if (strcmp(temp, ".extern") == 0) {
        return EXTERN;
    }
	return ERROR;
}

int fetchAddressType(char *operand, int line) {
	if (operand[0] == '\0')
		return NO_ADDRESS;
	if (operand[0] == 'r' && isdigit(operand[1]) && operand[2] == '\0')
		return REGISTER;
	if (operand[0] == 'r' && atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0')
		return REGISTER;
	if (operand[0] == '#' && isInteger(operand + 1))
		return IMMEDIATE;
	if (operand[0] && isIndex(operand, line))
		return INDEX;
	if (isSymbolNameValid(operand, line)) 
		return DIRECT;
	return NO_ADDRESS;
}

unsigned int fetchRegister(char *operand) {
	unsigned int reg;
	int index=0, j;
	char temp[MAX_LINE_LENGTH];

	if (operand[0] == 'r' && isdigit(operand[1]) && operand[2] == '\0') {
		reg = atoi(&operand[1]);
		if (reg >= 0 && reg <= 9)
			return reg;
	}

	if (operand[0] == 'r' && atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0') {
		reg = atoi(&operand[1]);
		return reg;
	}

	INCREASE_I_UNTILL_CHAR(operand, '[', index);
	index++;
	for (j=0; operand[index] && operand[index] != ']'; index++, j++) {
            temp[j] = operand[index];
        }
	temp[j] = '\0';

	return atoi(temp);
}

codeWord *generateFirstCodeWord(assemblyStructure *opcodeData) {
	codeWord *resWord;
	resWord = (codeWord *) safeMalloc(sizeof(codeWord));
	resWord->ARE = 4;
	resWord->L = 1;
	resWord->sourceAddress = resWord->sourceRegister = resWord->destinationAddress = resWord->destinationRegister = resWord->opcode = resWord->funct = 0;
	
	resWord->opcode = opcodeData->opcode;
	return resWord;
}

codeWord *generateSecondCodeWord(int line, char* lineContent, assemblyStructure *opcodeData, char **operandsArray, int numOfOperands) {
	codeWord *resWord;
	int address1 = NO_ADDRESS;
	int address2 = NO_ADDRESS;

	resWord = (codeWord *) safeMalloc(sizeof(codeWord));
	resWord->sourceAddress = resWord->sourceRegister = resWord->destinationAddress = resWord->destinationRegister = resWord->opcode = 0;
	resWord->L = 1;
	resWord->line = line;
	resWord->ARE = 4;
	resWord->funct = opcodeData->funct;
	if ( numOfOperands > 0) {
		address1 = fetchAddressType(operandsArray[0], line);
	}
	if ( numOfOperands > 1) {
		address2 = fetchAddressType(operandsArray[1], line);
	}

	/* validate the operands against the given function */
	if (!validateOperands(line, address1, address2, numOfOperands, opcodeData)) {
		return NULL;
	}
	
	if (opcodeData->numOfOperandsPerFunction == 2) {
		resWord->firstOperand = operandsArray[0];
		resWord->secondOperand = operandsArray[1];
		resWord->sourceAddress = address1;
		resWord->destinationAddress = address2;
		if ( address1 == REGISTER) {
			resWord->sourceRegister = fetchRegister(operandsArray[0]);
		}
		if ( address2 == REGISTER) {
			resWord->destinationRegister = fetchRegister(operandsArray[1]);
		}
		if ( address1 == IMMEDIATE) {
			resWord->L += 1;
		}
		if ( address2 == IMMEDIATE) {
			resWord->L += 1;
		}
		if ( address1 == DIRECT ) {
			resWord->L += 2;
		}
		if (address1 == INDEX) {
			resWord->L += 2;
			resWord->sourceRegister = fetchRegister(operandsArray[0]);
		}
		if ( address2 == DIRECT ) {
			resWord->L += 2;
		}
		if (address2 == INDEX) {
			resWord->L += 2;
			resWord->destinationRegister = fetchRegister(operandsArray[1]);
		}
	}
	else if (opcodeData->numOfOperandsPerFunction == 1) {
		resWord->firstOperand = operandsArray[0];
		resWord->destinationAddress = address1;
		if ( address1 == REGISTER) {
			resWord->destinationRegister = fetchRegister(operandsArray[0]);
		}
		if ( address1 == IMMEDIATE) {
			resWord->L += 1;
		}
		if ( address1 == DIRECT) {
			resWord->L += 2;
		}
		if  (address1 == INDEX) {
			resWord->L += 2;
			resWord->destinationRegister = fetchRegister(operandsArray[0]);
		}
	}

	return resWord;
}

int calculateBase(int ic) {
    int i;
    for (i = ic; i > 0; i--)
    {
        if (i % 16 == 0)
        {
            return i;
        }
    }
    return 0;   
}

int calculateOffset(int ic) {
    return ic - calculateBase(ic);
}

int validateString(int line, char *lineContent, int i) {

	if (lineContent[i] != '"') {
		printErrorMessage(line, "string type should start with quotes");
		return FALSE;
	}

	if (lineContent[i+1] == '"') {
		printErrorMessage(line, "String should not be empty!");
		return FALSE;
	}
	i++;

	for (; lineContent[i] != '"' && lineContent[i] != EOF && lineContent[i] != '\n'; i++);
	if (lineContent[i] == '"')
		return TRUE;

	return TRUE;
}
