
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firstPhase.h"

int handle_data(int lineIndex, char *line_content, int i) {
	int j;
	char data_param[MAX_LINE_LEN];
	data_word *data_to_add;

	FIND_NEXT_CHAR(line_content,i);

	if (line_content[i] == '\0' || line_content[i] == '\n')
		return TRUE;

	if (line_content[i] == ',')
		i++;

	FIND_NEXT_CHAR(line_content,i);

	for (j = 0; line_content[i] && line_content[i] != ',' && line_content[i] != '\n' && line_content[i] != '\t' && line_content[i] != ' ' && line_content[i] != EOF; i++, j++) {
		data_param[j] = line_content[i];
	}
	data_param[j] = '\0';
	
	if(atoi(data_param)){
		data_to_add = (data_word *) malloc(sizeof(data_word));
		data_to_add->data = atoi(data_param);
		machine_data_section[DC] = *data_to_add;
		free(data_to_add);
		DC++;
	}
	else {
		print_error_message(lineIndex, "Invalid data provided");
		return FALSE;
	}

	return handle_data(lineIndex,line_content,i);

}

int handle_string(int lineIndex, char *line_content, int i) {
	data_word *data_to_add;
	data_to_add = (data_word *) malloc(sizeof(data_word));
	

	if (line_content[i] == '"') {
		data_to_add->data = '\0';
		machine_data_section[DC] = *data_to_add;
		free(data_to_add);
		DC++;
		return TRUE;	
	}

	data_to_add->data = line_content[i];
	machine_data_section[DC] = *data_to_add;
	free(data_to_add);
	i++;
	
	DC++;
	return handle_string(lineIndex,line_content,i);
}

int handle_code(int lineIndex, char *line_content, int i) {
	int j, num_of_operands=0;
	char *operands_array[2];
	assembly_structure *opcode_data;
	code_word *first_word;
	code_word *second_word;
	/*
	code_word *direct_address_word;
	*/
	char function_name[MAX_LINE_LEN];

	for (j = 0; line_content[i] && line_content[i] != '\n' && line_content[i] != '\t' && line_content[i] != ' ' && line_content[i] != EOF; i++, j++) {
		function_name[j] = line_content[i];
	}
	function_name[j] = '\0';
	opcode_data = fetch_function_data(function_name);
	if(!opcode_data) {
		print_error_message(lineIndex, "could not find this assembly command");
		return FALSE;
	}

	FIND_NEXT_CHAR(line_content, i); 

	if (!fetch_operands(lineIndex, line_content, i, operands_array, &num_of_operands))  {
		return FALSE;
	}

	second_word = generate_second_code_word(lineIndex, line_content, opcode_data, operands_array, num_of_operands);
	if (!second_word) {
		return FALSE;
	}

	first_word = generate_first_code_word(opcode_data);

	machine_code_section[IC-IC_INIT_VALUE] = *first_word;
	machine_code_section[(IC+1)-IC_INIT_VALUE] = *second_word;

	if (num_of_operands == 0) {
		machine_code_section[IC-IC_INIT_VALUE] = *first_word;
		IC += first_word->L;
	}
	else {
		machine_code_section[IC-IC_INIT_VALUE] = *first_word;
		machine_code_section[(IC+1)-IC_INIT_VALUE] = *second_word;
		IC += first_word->L + second_word->L;
	}

	return TRUE;
}


int parseLine(int lineIndex, char *lineContent, symbolTable* table) {
    char extern_symbol[MAX_LINE_LEN];
	int data_type;
	int i=0, j;
	char symbol_name[MAX_LINE_LEN];

	FIND_NEXT_CHAR(lineContent, i); 
	if (!lineContent[i] || lineContent[i] == '\n' || lineContent[i] == EOF || lineContent[i] == ';')
		return TRUE; 
	
	if (fetch_symbol(lineIndex, lineContent, symbol_name)) {
		return FALSE;
	}
	if (THERE_IS_SYMBOL) {
		if (symbol_exists_in_table(symbol_name, table)) {
			print_error_message(lineIndex, "Symbol is already defined.");
			return FALSE;
		}
		INCREASE_I_UNTILL_CHAR(lineContent, ':', i);
	}
	if (lineContent[i] == '\n') {
		return TRUE; 
	}

	FIND_NEXT_CHAR(lineContent, i);
	data_type = fetch_type(lineContent, &i);
	if (data_type == FAILED_TO_FIND) {
		return FALSE;
	}

	FIND_NEXT_CHAR(lineContent, i);

	if ( data_type == CODE ) {
		if (THERE_IS_SYMBOL) {
			symbol_table_append(symbol_name, CODE, table);
		}
		if (!handle_code(lineIndex, lineContent, i))
			return FALSE;

	}
	else {
		if (data_type == DATA || data_type == STRING) {
			if (THERE_IS_SYMBOL){
				symbol_table_append(symbol_name, DATA, table);
			}
			if (data_type == DATA) {
				if (!handle_data(lineIndex, lineContent, i))
					return FALSE;
			}
			else {
				if (!validate_string(lineIndex, lineContent, i))
					return FALSE;
				if (!handle_string(lineIndex, lineContent, i+1))
					return FALSE;
			}	
		}
		else if (data_type == EXTERN) {
			for (j = 0; lineContent[i] && lineContent[i] != '\n' && lineContent[i] != ' ' && lineContent[i] != '\t' && lineContent[i] != EOF; i++, j++) {
				extern_symbol[j] = lineContent[i];
			}
			extern_symbol[j] = '\0';
			/* If invalid external label name, it's an error */
			if (!validate_symbol_name(extern_symbol, lineIndex)) {
				print_error_message(lineIndex, "Invalid symbol for extern type");
				return FALSE;
			}
			if (!symbol_exists_in_table(extern_symbol, table)){
				symbol_table_append(extern_symbol, EXTERN, table); /* Extern value is defaulted to 0 */
			}
		}
	}
   	
	return TRUE;
}

int runFirstPhase(FILE* fileAfterMacroParsing, symbolTable* table) {
	int lineIndex;
	char lineContent[MAX_LINE_WITH_LINEDROP_LEN];
	fgets(lineContent, MAX_LINE_WITH_LINEDROP_LEN, fileAfterMacroParsing);
	for (lineIndex = 0; fgets(lineContent, MAX_LINE_WITH_LINEDROP_LEN, fileAfterMacroParsing); lineIndex++) {
		int lineParseSuccess = parseLine(lineIndex, lineContent, table);
		if(!lineParseSuccess) {
			return FALSE;
		}
	}
	print_symbol_table(table);
	return TRUE;
}
