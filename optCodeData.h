#ifndef _GENERALS_H
#define _GENERALS_H

enum dataTypes {CODE, DATA, STRING, ENTRY, EXTERN, DATA_AND_ENTRY, CODE_AND_ENTRY, ERROR};
enum addressesTypes {IMMEDIATE, DIRECT, INDEX, REGISTER, NO_ADDRESS};
enum {SOURCE, DESTINATION};

/* define data instruction Struct */
typedef struct dataInstruction {
	int payload;
} dataInstruction;

typedef struct optCodeData {
	char* name;
	int numOfOperandsPerFunction;
	unsigned int opcode;
    unsigned int funct;
	int destinationOperandTypes[5];
	int sourceOperandTypes[5];
} optCodeData;

/* define code instruction Struct */
typedef struct codeInstruction {
	unsigned int opcode: 16;

	unsigned int destinationRegister: 4;
	unsigned int destinationAddress: 2;
	unsigned int sourceRegister: 4;
	unsigned int sourceAddress: 2;
	unsigned int funct: 4;
	char *firstOperand;
	char *secondOperand;

	unsigned int ARE: 3;
	int lineIndex;
	int L;

	unsigned int offset: 16;
	unsigned int base: 16;
	unsigned int immediate: 16;
} codeInstruction;


optCodeData* fetchFunctionData(char *function);
int isReservedWord(char *word);
int validateOperands(int line, int address1, int address2, int numOfOperands, optCodeData *opcodeData);

#endif
