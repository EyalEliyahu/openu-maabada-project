
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

char *reserved_words[NUM_OF_RESERVED_WORDS] = {
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

int IC = IC_INIT_VALUE;
int DC = DC_INIT_VALUE;
int ICF = 0;
int DCF = 0; 

int isReservedWord(char *word, int line) {
	int i; 

	for(i = 0; i < NUM_OF_RESERVED_WORDS; i++) 
	{	
		if (strcmp(word,reserved_words[i]) == 0) {
		return TRUE;
		}
	}	
	return FALSE;
}	

assemblyStructure* fetchFunctionData(char *functionName) {
	int i;
	for (i = 0; i < NUM_OF_OPCODES; i++)
	{
		if (strcmp(functionName, assemblyStructureTable[i].name) == 0) {
			return &assemblyStructureTable[i];
		}
	}
	return NULL;
}

void printMachineCode() { /* used for debugging */
	int i;
	printf("------------------------ CODE ---------------------\n");
	for (i=0; i < IC - IC_INIT_VALUE; i++)
	{
		printf("IC: %d | ARE: %d | L: %d | opcode: %d | funct: %d | sourceRegister: %d | sourceAddress: %d | destinationRegister: %d | destinationAddress: %d | firstOperand: %s | secondOperand: %s | immediate: %d | base: %d | offset: %d\n", 
		i+IC_INIT_VALUE, machineCodeSection[i].ARE, machineCodeSection[i].L, machineCodeSection[i].opcode, machineCodeSection[i].funct, machineCodeSection[i].sourceRegister,
		machineCodeSection[i].sourceAddress, machineCodeSection[i].destinationRegister, machineCodeSection[i].destinationAddress, machineCodeSection[i].firstOperand, machineCodeSection[i].secondOperand,
		machineCodeSection[i].immediate, machineCodeSection[i].base, machineCodeSection[i].offset);
	}
	printf("\n----------------------- DATA ---------------------\n");
	for (i=0; i < DC; i++)
	{
		printf("DC: %d | data: %d\n", i, machineDataSection[i].data);
	}	
}

int validate_address_type(int line, assemblyStructure *opcodeData, int address, int source_or_destination)
{
	int i;
	if (source_or_destination == SOURCE) {
		for (i = 0; opcodeData->sourceOperandTypes[i] != -1; i++) {
			if (opcodeData->sourceOperandTypes[i] == address) 
				return TRUE;	
		}
	}

	if (source_or_destination == DESTINATION) {
		for (i = 0; opcodeData->destinationOperandTypes[i] != -1; i++) {
			if (opcodeData->destinationOperandTypes[i] == address) 
				return TRUE;	
		}
	}
	printErrorMessage(line, "The addressing type provided is incompatible with this assembly command");
	return FALSE;
}

int validateOperands(int line, int address1, int address2, int numOfOperands, assemblyStructure *opcodeData) {
	if (opcodeData->numOfOperandsPerFunction != numOfOperands) {
		printErrorMessage(line, "Invalid amount of operands for this assembly command");
		return FALSE;
	}
	if (numOfOperands == 2) {
		return validate_address_type(line, opcodeData, address1, SOURCE) && validate_address_type(line, opcodeData, address2, DESTINATION);
	}
	else if (numOfOperands == 1) {
		return validate_address_type(line, opcodeData, address1, DESTINATION);
	}
	return TRUE;	
}
