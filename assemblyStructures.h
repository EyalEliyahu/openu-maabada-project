#ifndef _GENERALS_H
#define _GENERALS_H

#define MAX_LINE_LEN 80
#define MAX_LINE_WITH_LINEDROP_LEN 82
#define IC_INIT_VALUE 100
#define DC_INIT_VALUE 0
#define MAX_SYMBOL_SIZE 31
#define MAX_MACHINE_CODE_SECTION 8192
#define MAX_MACHINE_DATA_SECTION 8192
#define NUM_OF_RESERVED_WORDS 36
#define NUM_OF_OPCODES 16

enum dataTypes {CODE, DATA, STRING, ENTRY, EXTERN, DATAANDENTRY, CODEANDENTRY, FAILED_TO_FIND};
enum addresses_types {IMMEDIATE, DIRECT, INDEX, REGISTER, NO_ADDRESS, FAILED_ADDRESS};
enum {SOURCE, DESTINATION};

/* struct for holding an opcode and it's binary value */
typedef struct assemblyStructure {
	char* name;
	unsigned int opcode;
    unsigned int funct;
	int numOfOperandsPerFunction;
	int sourceOperandTypes[5];
	int destinationOperandTypes[5];
} assemblyStructure;

/* Code Word Struct */
typedef struct codeWord {
	/* shared to all words */
	unsigned int ARE: 3;
	int L;
	int line;

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

} codeWord;

/* DATA Word Struct */
typedef struct dataWord {
	int data;
} dataWord;

int isReservedWord(char *word, int line);
assemblyStructure* fetchFunctionData(char *function);
void printMachineCode();
int validateOperands(int line, int address1, int address2, int numOfOperands, assemblyStructure *opcodeData);

extern assemblyStructure assemblyStructureTable[];
extern codeWord machineCodeSection[];
extern dataWord machineDataSection[];
extern int IC;
extern int DC;
extern int ICF;
extern int DCF;

#endif
