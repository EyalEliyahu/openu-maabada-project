
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assemblyStructures.h"
#include "utils.h"

code_word machine_code_section[MAX_MACHINE_CODE_SECTION];
data_word machine_data_section[MAX_MACHINE_DATA_SECTION];

assembly_structure assembly_structure_table[] = {
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

int is_reserved_word(char *word, int line) {
	int i; 

	for(i = 0; i < NUM_OF_RESERVED_WORDS; i++) 
	{	
		if (strcmp(word,reserved_words[i]) == 0) {
		return TRUE;
		}
	}	
	return FALSE;
}	

assembly_structure* fetch_function_data(char *function_name) {
	int i;
	for (i = 0; i < NUM_OF_OPCODES; i++)
	{
		if (strcmp(function_name, assembly_structure_table[i].name) == 0) {
			return &assembly_structure_table[i];
		}
	}
	return NULL;
}

void print_machine_code() { /* used for debugging */
	int i;
	printf("------------------------ CODE ---------------------\n");
	for (i=0; i < IC - IC_INIT_VALUE; i++)
	{
		printf("IC: %d | ARE: %d | L: %d | opcode: %d | funct: %d | source_reg: %d | source_address: %d | destination_reg: %d | destination_address: %d | first_operand: %s | second_operand: %s | immediate: %d | base: %d | offset: %d\n", 
		i+IC_INIT_VALUE, machine_code_section[i].ARE, machine_code_section[i].L, machine_code_section[i].opcode, machine_code_section[i].funct, machine_code_section[i].source_reg,
		machine_code_section[i].source_address, machine_code_section[i].destination_reg, machine_code_section[i].destination_address, machine_code_section[i].first_operand, machine_code_section[i].second_operand,
		machine_code_section[i].immediate, machine_code_section[i].base, machine_code_section[i].offset);
	}
	printf("\n----------------------- DATA ---------------------\n");
	for (i=0; i < DC; i++)
	{
		printf("DC: %d | data: %d\n", i, machine_data_section[i].data);
	}	
}

int validate_address_type(int line, assembly_structure *opcode_data, int address, int source_or_destination)
{
	int i;
	if (source_or_destination == SOURCE) {
		for (i = 0; opcode_data->source_operand_types[i] != -1; i++) {
			if (opcode_data->source_operand_types[i] == address) 
				return TRUE;	
		}
	}

	if (source_or_destination == DESTINATION) {
		for (i = 0; opcode_data->destination_operand_types[i] != -1; i++) {
			if (opcode_data->destination_operand_types[i] == address) 
				return TRUE;	
		}
	}
	print_error_message(line, "The addressing type provided is incompatible with this assembly command");
	return FALSE;
}

int validate_operands(int line, int address1, int address2, int num_of_operands, assembly_structure *opcode_data) {
	if (opcode_data->num_of_operands_per_function != num_of_operands) {
		print_error_message(line, "Invalid amount of operands for this assembly command");
		return FALSE;
	}
	if (num_of_operands == 2) {
		return validate_address_type(line, opcode_data, address1, SOURCE) && validate_address_type(line, opcode_data, address2, DESTINATION);
	}
	else if (num_of_operands == 1) {
		return validate_address_type(line, opcode_data, address1, DESTINATION);
	}
	return TRUE;	
}
