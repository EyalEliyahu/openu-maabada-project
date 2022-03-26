
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assemblyStructures.h"
#include "utils.h"

codeWord machineCodeSection[MAX_MACHINE_CODE_SECTION];
dataWord machineDataSection[MAX_MACHINE_DATA_SECTION];

assemblyStructure assemblyStructureTable[] = {
	{ "mov", 1, 0, 2, {0,1,2,3,-1}, {1,2,3,-1}}, /* opcode = 0000000000000001 = 1 */
	{ "cmp", 2, 0, 2, {0,1,2,3,-1}, {0,1,2,3,-1}}, /* opcode = 0000000000000010 = 2 */
	{ "add", 4, 10, 2, {0,1,2,3,-1}, {1,2,3,-1}}, /* opcode = 0000000000000100 = 4 */
	{ "sub", 4, 11, 2, {0,1,2,3,-1}, {1,2,3,-1}}, /* opcode = 0000000000000100 = 4 */
	{ "lea", 16, 0, 2, {1,2,-1}, {1,2,3,-1}}, /* opcode = 0000000000010000 = 16 */
	{ "clr", 32, 10, 1, {-1}, {1,2,3,-1}}, /* opcode = 0000000000100000 = 32 */
	{ "not", 32, 11, 1, {-1}, {1,2,3,-1}}, /* opcode = 0000000000100000 = 32 */
	{ "inc", 32, 12, 1, {-1}, {1,2,3,-1}}, /* opcode = 0000000000100000 = 32 */
	{ "dec", 32, 13, 1, {-1}, {1,2,3,-1}}, /* opcode = 0000000000100000 = 32 */
	{ "jmp", 512, 10, 1, {-1}, {1,2,-1}}, /* opcode = 0000001000000000 = 512 */
	{ "bne", 512, 11, 1, {-1}, {1,2,-1}}, /* opcode = 0000001000000000 = 512 */
	{ "jsr", 512, 12, 1, {-1}, {1,2,-1}}, /* opcode = 0000001000000000 = 512 */
	{ "red", 4096, 0, 1, {-1}, {1,2,3,-1}}, /* opcode = 0001000000000000 = 4096 */
	{ "prn", 8192, 0, 1, {-1}, {0,1,2,3,-1}}, /* opcode = 0010000000000000 = 8192 */
	{ "rts", 16384, 0, 0, {-1}, {-1}}, /* opcode = 0100000000000000 = 16384 */
	{ "stop", -32768, 0, 0, {-1}, {-1}} /* opcode = 1000000000000000 = -32768 */
};

char *reservedWords[NUM_OF_RESERVED_WORDS] = {
	".extern",
	".entry", 
	".data", 
	".string", 
	"mov", 
	"cmp",
	"add", 
	"sub", 
	"not", 
	"clr", 
	"lea", 				
	"inc", 
	"dec", 
	"jmp", 
	"bne", 
	"red", 
	"prn", 
	"jsr", 
	"rts", 
	"stop", 
	"r0", 
	"r1", 
	"r2", 
	"r3", 
	"r4", 
	"r5", 
	"r6", 
	"r7", 
	"r8", 
	"r9", 
	"r10", 
	"r11",
	"r12",
	"r13",
	"r14",
	"r15"};

int isReservedWord(char *word, int line) {
	int indexInWord; 

	for(indexInWord = 0; indexInWord < NUM_OF_RESERVED_WORDS; indexInWord++) 
	{	
		if (IS_STR_EQL(word,reservedWords[indexInWord])) {
		return TRUE;
		}
	}	
	return FALSE;
}	

int validate_address_type(int line, assemblyStructure *opcodeData, int addressType, int source_or_destination)
{
	int operandIndex;
	if (source_or_destination == SOURCE) {
		for (operandIndex = 0; opcodeData->sourceOperandTypes[operandIndex] != -1; operandIndex++) {
			if (opcodeData->sourceOperandTypes[operandIndex] == addressType) 
				return TRUE;	
		}
	}

	if (source_or_destination == DESTINATION) {
		for (operandIndex = 0; opcodeData->destinationOperandTypes[operandIndex] != -1; operandIndex++) {
			if (opcodeData->destinationOperandTypes[operandIndex] == addressType) 
				return TRUE;	
		}
	}
	printLineError(line, "The addressing type provided is incompatible with this assembly command");
	return FALSE;
}

int validateOperands(int line, int address1, int address2, int numOfOperands, assemblyStructure *opcodeData) {
	if (opcodeData->numOfOperandsPerFunction != numOfOperands) {
		printLineError(line, "Invalid amount of operands for this assembly function. Expected: %d | Received: %d", opcodeData->numOfOperandsPerFunction, numOfOperands);
		return FALSE;
	}
	if (numOfOperands == 1) {
		return validate_address_type(line, opcodeData, address1, DESTINATION);

	}
	else if (numOfOperands == 2) {
		return validate_address_type(line, opcodeData, address1, SOURCE) && validate_address_type(line, opcodeData, address2, DESTINATION);
	}
	
	return TRUE;	
}

assemblyStructure* fetchFunctionData(char *functionName) {
	int opcodeIndex;
	for (opcodeIndex = 0; opcodeIndex < NUM_OF_OPCODES; opcodeIndex++)
	{
		if (IS_STR_EQL(functionName, assemblyStructureTable[opcodeIndex].name)) {
			return &assemblyStructureTable[opcodeIndex];
		}
	}
	return NULL;
}
