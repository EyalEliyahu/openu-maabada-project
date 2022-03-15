
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firstPhase.h"

int handle_data(int lineIndex, char *lineContent, int i) {
	int j;
	char data_param[MAX_LINE_LEN];
	data_word *data_to_add;

	FIND_NEXT_CHAR(lineContent,i);

	if (lineContent[i] == '\0' || lineContent[i] == '\n')
		return TRUE;

	if (lineContent[i] == ',')
		i++;

	FIND_NEXT_CHAR(lineContent,i);

	for (j = 0; lineContent[i] && lineContent[i] != ',' && lineContent[i] != '\n' && lineContent[i] != '\t' && lineContent[i] != ' ' && lineContent[i] != EOF; i++, j++) {
		data_param[j] = lineContent[i];
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

	return handle_data(lineIndex,lineContent,i);

}

int handle_string(int lineIndex, char *lineContent, int i) {
	data_word *data_to_add;
	data_to_add = (data_word *) malloc(sizeof(data_word));
	

	if (lineContent[i] == '"') {
		data_to_add->data = '\0';
		machine_data_section[DC] = *data_to_add;
		free(data_to_add);
		DC++;
		return TRUE;	
	}

	data_to_add->data = lineContent[i];
	machine_data_section[DC] = *data_to_add;
	free(data_to_add);
	i++;
	
	DC++;
	return handle_string(lineIndex,lineContent,i);
}

int handle_code(int lineIndex, char *lineContent, int i) {
	int j, num_of_operands=0;
	char *operands_array[2];
	assembly_structure *opcode_data;
	code_word *first_word;
	code_word *second_word;
	/*
	code_word *direct_address_word;
	*/
	char function_name[MAX_LINE_LEN];

	for (j = 0; lineContent[i] && lineContent[i] != '\n' && lineContent[i] != '\t' && lineContent[i] != ' ' && lineContent[i] != EOF; i++, j++) {
		function_name[j] = lineContent[i];
	}
	function_name[j] = '\0';
	opcode_data = fetch_function_data(function_name);
	if(!opcode_data) {
		print_error_message(lineIndex, "could not find this assembly command");
		return FALSE;
	}

	FIND_NEXT_CHAR(lineContent, i); 

	if (!fetch_operands(lineIndex, lineContent, i, operands_array, &num_of_operands))  {
		return FALSE;
	}

	second_word = generate_second_code_word(lineIndex, lineContent, opcode_data, operands_array, num_of_operands);
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
    char externSymbol[MAX_LINE_LEN];
	int dataType;
	int i=0, j;
	char symbolName[MAX_LINE_LEN];

	FIND_NEXT_CHAR(lineContent, i); 
	if (!lineContent[i] || lineContent[i] == '\n' || lineContent[i] == EOF || lineContent[i] == ';')
		return TRUE; 
	
	if (fetch_symbol(lineIndex, lineContent, symbolName)) {
		return FALSE;
	}
	if (THERE_IS_SYMBOL) {
		if (symbolExistsInTable(symbolName, table)) {
			print_error_message(lineIndex, "Symbol is already defined.");
			return FALSE;
		}
		INCREASE_I_UNTILL_CHAR(lineContent, ':', i);
	}
	if (lineContent[i] == '\n') {
		return TRUE; 
	}

	FIND_NEXT_CHAR(lineContent, i);
	dataType = fetch_type(lineContent, &i);
	if (dataType == FAILED_TO_FIND) {
		return FALSE;
	}

	FIND_NEXT_CHAR(lineContent, i);

	if ( dataType == CODE ) {
		if (THERE_IS_SYMBOL) {
			symbolTableAppend(symbolName, CODE, table);
		}
		if (!handle_code(lineIndex, lineContent, i))
			return FALSE;

	}
	else {
		if (dataType == DATA || dataType == STRING) {
			if (THERE_IS_SYMBOL){
				symbolTableAppend(symbolName, DATA, table);
			}
			if (dataType == DATA) {
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
		else if (dataType == EXTERN) {
			for (j = 0; lineContent[i] && lineContent[i] != '\n' && lineContent[i] != ' ' && lineContent[i] != '\t' && lineContent[i] != EOF; i++, j++) {
				externSymbol[j] = lineContent[i];
			}
			externSymbol[j] = '\0';
			/* If invalid external label name, it's an error */
			if (!validateSymbolName(externSymbol, lineIndex)) {
				print_error_message(lineIndex, "Invalid symbol for extern type");
				return FALSE;
			}
			if (!symbolExistsInTable(externSymbol, table)){
				symbolTableAppend(externSymbol, EXTERN, table); /* Extern value is defaulted to 0 */
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
	printSymbolTable(table);
	return TRUE;
}
