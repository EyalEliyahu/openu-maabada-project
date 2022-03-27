#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "optCodeData.h"
#include "utils.h"

/* define reserved words*/
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
	"r15"
};

/*This function return true if the given word is reserved , else false*/
int isReservedWord(char *word) {
	int indexInWord; 

	for(indexInWord = 0; indexInWord < NUM_OF_RESERVED_WORDS; indexInWord++) 
	{	
		if (IS_STR_EQL(word,reservedWords[indexInWord])) {
		    return TRUE;
		}
	}	
	return FALSE;
}	

/* define opt code data list*/
optCodeData optCodeDataList[] = {
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

/* This function validate address type*/
int validateAddressType(int lineIndex, optCodeData *opcodeData, int address, int type)
{
	int i;
	if (type == SOURCE) {
		for (i = 0; opcodeData->sourceOperandTypes[i] != -1; i++) {
			if (opcodeData->sourceOperandTypes[i] == address) 
				return TRUE;	
		}
	}

	if (type == DESTINATION) {
		for (i = 0; opcodeData->destinationOperandTypes[i] != -1; i++) {
			if (opcodeData->destinationOperandTypes[i] == address) 
				return TRUE;	
		}
	}
	printLineError(lineIndex, "The provided address type is not valid to this assembly command");
	return FALSE;
}

/* This function validate operands*/
int validateOperands(int lineIndex, int address1, int address2, int numOfOperands, optCodeData *opcodeData) {
	if (opcodeData->numOfOperandsPerFunction != numOfOperands) {
		printLineError(lineIndex, "The amount of received operands don't compatible with the expected for this assembly function. Expected: %d | Received: %d", opcodeData->numOfOperandsPerFunction,  numOfOperands);
		return FALSE;
	}
	if (numOfOperands == 1) {
			return validateAddressType(lineIndex, opcodeData, address1, DESTINATION);
	}
	else if (numOfOperands == 2) {
		return validateAddressType(lineIndex, opcodeData, address1, SOURCE) && validateAddressType(lineIndex, opcodeData, address2, DESTINATION);
	}
	return TRUE;	
}

/* This function responsible to fetch function data*/
optCodeData* fetchFunctionData(char *functionName) {
	int optCodeIndex;
	for (optCodeIndex = 0; optCodeIndex < NUM_OF_OPCODES; optCodeIndex++)
	{
		if (strcmp(functionName, optCodeDataList[optCodeIndex].name) == 0) {
			return &optCodeDataList[optCodeIndex];
		}
	}
	return NULL;
}
