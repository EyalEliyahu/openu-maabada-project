#ifndef _GENERALS_H
#define _GENERALS_H


enum dataTypes {CODE, DATA, STRING, ENTRY, EXTERN, DATA_AND_ENTRY, CODE_AND_ENTRY, ERROR};
enum addressesTypes {IMMEDIATE, DIRECT, INDEX, REGISTER, NO_ADDRESS};
enum {SOURCE, DESTINATION};

/* struct for holding an opcode and it's binary value */
typedef struct optCodeData {
	char* name;
	unsigned int opcode;
    unsigned int funct;
	int numOfOperandsPerFunction;
	int sourceOperandTypes[5];
	int destinationOperandTypes[5];
} optCodeData;

/* Code Word Struct */
typedef struct codeInstruction {
	/* shared to all words */
	unsigned int ARE: 3;
	int L;
	int lineIndex;

	/* first word */
	unsigned int opcode: 16;

	/* second word */
	unsigned int funct: 4;
	unsigned int sourceRegister: 4;
	unsigned int sourceAddress: 2;
	unsigned int destinationRegister: 4;
	unsigned int destinationAddress: 2;

	/* operands */
	char *firstOperand;
	char *secondOperand;

	/* immediate address */
	unsigned int immediate: 16;

	/* index + direct addresses */
	unsigned int base: 16;
	unsigned int offset: 16;

} codeInstruction;

/* DATA Word Struct */
typedef struct dataInstruction {
	int data;
} dataInstruction;

int isReservedWord(char *word);
optCodeData* fetchFunctionData(char *function);
int validateOperands(int line, int address1, int address2, int numOfOperands, optCodeData *opcodeData);

extern optCodeData optCodeDataList[];
extern codeInstruction codeInstructionsList[];
extern dataInstruction dataInstructionsList[];

#endif
