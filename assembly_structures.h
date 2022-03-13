#ifndef _GENERALS_H
#define _GENERALS_H

#define IC_INIT_VALUE 100
#define DC_INIT_VALUE 0
#define MAX_SYMBOL_SIZE 31
#define MAX_MACHINE_CODE_SECTION 8192
#define MAX_MACHINE_DATA_SECTION 8192
#define NUM_OF_RESERVED_WORDS 36
#define NUM_OF_OPCODES 16

enum data_types {CODE, DATA, STRING, ENTRY, EXTERN, DATAANDENTRY, CODEANDENTRY, FAILED_TO_FIND};
enum addresses_types {IMMEDIATE, DIRECT, INDEX, REGISTER, NO_ADDRESS, FAILED_ADDRESS};
enum {SOURCE, DESTINATION};

/* struct for holding an opcode and it's binary value */
typedef struct assembly_structure {
	char* name;
	unsigned int opcode;
    unsigned int funct;
	int num_of_operands_per_function;
	int source_operand_types[5];
	int destination_operand_types[5];
} assembly_structure;

/* Code Word Struct */
typedef struct code_word {
	/* shared to all words */
	unsigned int ARE: 3;
	int L;
	int line;

	/* first word */
	unsigned int opcode: 16;

	/* second word */
	unsigned int funct: 4;
	unsigned int source_reg: 4;
	unsigned int source_address: 2;
	unsigned int destination_reg: 4;
	unsigned int destination_address: 2;

	/* operands */
	char *first_operand;
	char *second_operand;

	/* immediate address */
	unsigned int immediate: 16;

	/* index + direct addresses */
	unsigned int base: 16;
	unsigned int offset: 16;

} code_word;

/* DATA Word Struct */
typedef struct data_word {
	int data;
} data_word;

int is_reserved_word(char *word, int line);
assembly_structure* fetch_function_data(char *function);
void print_machine_code();
int validate_operands(int line, int address1, int address2, int num_of_operands, assembly_structure *opcode_data);

extern assembly_structure assembly_structure_table[];
extern code_word machine_code_section[];
extern data_word machine_data_section[];
extern int IC;
extern int DC;
extern int ICF;
extern int DCF;

#endif