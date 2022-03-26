#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "symbolTable.h"

#define ADD_ARE(addressType, operand) \
        if (addressType == EXTERN) {\
            machineCodeSection[*indexInLine+instructionIndex].ARE = 1;\
            machineCodeSection[*indexInLine+instructionIndex].firstOperand = operand;\
        }\
        else {\
            machineCodeSection[*indexInLine+instructionIndex].ARE = 2;\
        }

int updateCodeWordByType(int line, int instructionIndex, int* indexInLine, char* operand, int addressType, symbolTable* table) {
    int symbolNameIndex;
    char* symbolName;
    if (addressType == IMMEDIATE) {
        machineCodeSection[*indexInLine+instructionIndex].immediate = atoi(&operand[1]); 
        machineCodeSection[*indexInLine+instructionIndex].ARE = 3;
         (*indexInLine)++;
    }
    else if (addressType == DIRECT) {
        symbolItem *temp;
        temp = symbolItemInTable(operand, table);
        if(!temp) {
            printLineError(machineCodeSection[instructionIndex].line, "Could not find symbol: %s in the symbol table", operand);
            return FALSE;
        }
        machineCodeSection[instructionIndex+*indexInLine].base = temp->base;
        ADD_ARE(temp->symbolType, operand);
        (*indexInLine)++;
        machineCodeSection[instructionIndex+*indexInLine].offset = temp->offset;
        ADD_ARE(temp->symbolType, operand);
        (*indexInLine)++;
    }
    else if (addressType == INDEX) {
        symbolItem *temp;
        symbolName = safeMalloc(sizeof(char) * MAX_LINE_LENGTH);
        for (symbolNameIndex = 0; operand[symbolNameIndex] != '['; symbolNameIndex++) {
            symbolName[symbolNameIndex] = operand[symbolNameIndex];
        }
        symbolName[symbolNameIndex] = '\0';
        temp = symbolItemInTable(symbolName, table);
        if(!temp) {
            printLineError(machineCodeSection[instructionIndex].line, "Could not find symbol: %s in the symbol table", symbolName);
            return FALSE;
        }
        machineCodeSection[instructionIndex+*indexInLine].base = temp->base;
        ADD_ARE(temp->symbolType, symbolName);
        (*indexInLine)++;
        machineCodeSection[instructionIndex+*indexInLine].offset = temp->offset;
        ADD_ARE(temp->symbolType, symbolName);
        (*indexInLine)++;
    }

    return TRUE;
}

int parseLineForSecondPass(int lineIndex, char* lineContent, symbolTable* table) {
	int indexInLine=0, j;
    int dataType;
	char symbolName[MAX_LINE_WITH_LINEDROP_LEN];
    char entrySymbol[MAX_LINE_WITH_LINEDROP_LEN];

	INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine);
    if (IS_NULLISH_CHAR(lineContent[indexInLine]) || lineContent[indexInLine] == ';')
		return TRUE; 

    getSymbolFromLine(lineIndex, lineContent, symbolName);
	
	if (IS_STRING_EXISTS(symbolName))
        return TRUE;

    INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine)
	dataType = fetchType(lineContent, &indexInLine);
    INCREASE_INDEX_UNTILL_NEXT_CHAR(lineContent, indexInLine)
	
    if (dataType == ENTRY) {
        for (j = 0; !IS_NULLISH_CHAR(lineContent[indexInLine+j]);j++) {
            entrySymbol[j] = lineContent[indexInLine+j];
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
    /* update all of the code words that havent been coded in the first pass */
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
void validateMachineCodeLimitation(int IC, int DC, int lineIndex) {
	if (IC - IC_INIT_VALUE + DC > MAX_MACHINE_CODE_SECTION){
		printLineError(lineIndex, "The machine code is too big and can only include %d words", MAX_MACHINE_CODE_SECTION);
        exit(EXIT_FAILURE);
    }
}

int runSecondPass(FILE* fileAfterMacroParsing, symbolTable* table, int IC, int DC) {
    int lineIndex;
	char lineContent[MAX_LINE_WITH_LINEDROP_LEN];
	for (lineIndex = 1; fgets(lineContent, MAX_LINE_WITH_LINEDROP_LEN, fileAfterMacroParsing) != NULL; lineIndex++)
	{
		if (!parseLineForSecondPass(lineIndex, lineContent, table))
			return FALSE;
	}
    if (!updateCodeWords(IC, table))
        return FALSE;
        
	validateMachineCodeLimitation(IC, DC, lineIndex);
    return TRUE;
}
