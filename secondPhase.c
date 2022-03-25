#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "symbolTable.h"

#define ADD_ARE(addressType, operand) \
        if (addressType == EXTERN) {\
            machineCodeSection[*i+instructionIndex].ARE = 1;\
            machineCodeSection[*i+instructionIndex].firstOperand = operand;\
        }\
        else {\
            machineCodeSection[*i+instructionIndex].ARE = 2;\
        }

int updateCodeWordByType(int line, int instructionIndex, int *i, char* operand, int addressType, symbolTable* table) {
    int symbolNameIndex;
    char* symbolName;
    if (addressType == IMMEDIATE) {
        machineCodeSection[*i+instructionIndex].immediate = atoi(&operand[1]); 
        machineCodeSection[*i+instructionIndex].ARE = 3;
         (*i)++;
    }
    else if (addressType == DIRECT) {
        symbolItem *temp;
        temp = symbolItemInTable(operand, table);
        if(!temp) {
            printErrorMessage(machineCodeSection[instructionIndex].line, "Could not find symbol: %s in the symbol table", operand);
            return FALSE;
        }
        machineCodeSection[instructionIndex+*i].base = temp->base;
        ADD_ARE(temp->symbolType, operand);
        (*i)++;
        machineCodeSection[instructionIndex+*i].offset = temp->offset;
        ADD_ARE(temp->symbolType, operand);
        (*i)++;
    }
    else if (addressType == INDEX) {
        symbolItem *temp;
        symbolName = malloc(sizeof(char) * MAX_LINE_LENGTH);
        for (symbolNameIndex = 0; operand[symbolNameIndex] != '['; symbolNameIndex++) {
            symbolName[symbolNameIndex] = operand[symbolNameIndex];
        }
        symbolName[symbolNameIndex] = '\0';
        temp = symbolItemInTable(symbolName, table);
        if(!temp) {
            printErrorMessage(machineCodeSection[instructionIndex].line, "Could not find symbol: %s in the symbol table", symbolName);
            return FALSE;
        }
        machineCodeSection[instructionIndex+*i].base = temp->base;
        ADD_ARE(temp->symbolType, symbolName);
        (*i)++;
        machineCodeSection[instructionIndex+*i].offset = temp->offset;
        ADD_ARE(temp->symbolType, symbolName);
        (*i)++;
    }

    return TRUE;
}

int parseLineForSecondPhase(int lineIndex, char *lineContent, symbolTable* table) {
	int i=0, j;
    int dataType;
	char symbolName[MAX_LINE_WITH_LINEDROP_LEN];
    char entrySymbol[MAX_LINE_WITH_LINEDROP_LEN];

	INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i); 
    if (!lineContent[i] || lineContent[i] == '\n' || lineContent[i] == EOF || lineContent[i] == ';')
		return TRUE; 

    fetchSymbol(lineIndex, lineContent, symbolName);
	
	if (IS_STRING_EXISTS(symbolName) || lineContent[i] == '\n')
        return TRUE;

    INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i)
	dataType = fetchType(lineContent, &i);
    INCREASE_I_UNTILL_NEXT_CHAR(lineContent, i)
	
    if (dataType == ENTRY) {
        for (j = 0; lineContent[i+j] && lineContent[i+j] != '\n' && lineContent[i+j] != EOF;j++) {
            entrySymbol[j] = lineContent[i+j];
        }
        entrySymbol[j] = '\0';
        if (!updateSymbolWithEntryAttribute(entrySymbol, lineIndex, table))
            return FALSE;
    }

    return TRUE;
}

int updateCodeWords(int IC, symbolTable* table) {
    int lineIndex, instructionIndex, i, adressType;
    char* operand = NULL;
    codeWord instruction;
    int isMovInstuction, isAbsoluteARE;
    /* update all of the code words that havent been coded in the first phase */
	for (instructionIndex = 0, lineIndex = 0; instructionIndex < IC - IC_INIT_VALUE; instructionIndex++)
	{      
        instruction = machineCodeSection[instructionIndex];
        isMovInstuction = instruction.opcode == 0;
        isAbsoluteARE = instruction.ARE > 3;
		if (isMovInstuction && isAbsoluteARE) { /* run on all of the second code words */
			i = 1;
			lineIndex++;
			if (instruction.firstOperand && instruction.secondOperand) {
				if (instruction.sourceAddress != REGISTER) {
                    operand = instruction.firstOperand;
                    adressType = instruction.sourceAddress;
                    if (!updateCodeWordByType(lineIndex, instructionIndex, &i, operand, adressType, table))
			        	return FALSE;
				}
				if (instruction.destinationAddress != REGISTER) {
                    operand = instruction.secondOperand;
                    adressType = instruction.destinationAddress;
                    if (!updateCodeWordByType(lineIndex, instructionIndex, &i, operand, adressType, table))
			        	return FALSE;
				}
			}
			else if (instruction.destinationAddress != REGISTER) {
                    operand = instruction.firstOperand;
                    adressType = instruction.destinationAddress;
                    if (!updateCodeWordByType(lineIndex, instructionIndex, &i, operand, adressType, table))
			        	return FALSE;
			}
		}
	}
    return TRUE;
}
void validateMachineCodeLimitation(int IC, int DC) {
	if (IC - IC_INIT_VALUE + DC > MAX_machineCodeSection){
		printf("The machine code is too big and can only include %d words", MAX_machineCodeSection);
        exit(EXIT_FAILURE);
    }
}

int runSecondPhase(FILE* fileAfterMacroParsing, symbolTable* table, int IC, int DC) {
    int lineIndex;
	char lineContent[MAX_LINE_WITH_LINEDROP_LEN];
	for (lineIndex = 1; fgets(lineContent, MAX_LINE_WITH_LINEDROP_LEN, fileAfterMacroParsing) != NULL; lineIndex++)
	{
		if (!parseLineForSecondPhase(lineIndex, lineContent, table))
			return FALSE;
	}
    if (!updateCodeWords(IC, table))
        return FALSE;
        
	validateMachineCodeLimitation(IC, DC);
    return TRUE;
}
