#ifndef _GENERALS_H
#define _GENERALS_H

enum dataTypes {CODE, DATA, STRING, ENTRY, EXTERN, DATA_AND_ENTRY, CODE_AND_ENTRY, ERROR};
enum addressesTypes {IMMEDIATE, DIRECT, INDEX, REGISTER, NO_ADDRESS};
enum {SOURCE, DESTINATION};

/* define data instruction Struct */
typedef struct dataInstruction {
	int payload;
} dataInstruction;

/* define opcode data struct */
typedef struct optCodeData {
	char* name;
	unsigned int opcode;
    unsigned int funct;
	int numOfOperandsPerFunction;
	int sourceOperandTypes[5];
	int destinationOperandTypes[5];
} optCodeData;

/* define data instruction Struct */
typedef struct codeInstruction {	/* shared to all words */
	unsigned int ARE: 3;
	int L;
	int lineIndex;
	unsigned int opcode: 16;
	unsigned int funct: 4;
	unsigned int sourceRegister: 4;
	unsigned int sourceAddress: 2;
	unsigned int destinationRegister: 4;
	unsigned int destinationAddress: 2;
	char *firstOperand;
	char *secondOperand;
	unsigned int immediate: 16;
	unsigned int base: 16;
	unsigned int offset: 16;
} codeInstruction;


optCodeData* fetchFunctionData(char *function);
int isReservedWord(char *word);
int validateOperands(int line, int address1, int address2, int numOfOperands, optCodeData *opcodeData);

#endif
