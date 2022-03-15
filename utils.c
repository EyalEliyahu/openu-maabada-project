#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>
#include "utils.h"


char** getLineContent(char* lineString){
    return NULL;
    /*// TBD
    // int i = 0;
    // char **array = malloc (sizeof (char *) * NUMBER);
    // if (!array)
    //     return NULL;
    // for (i = 0; i < NUMBER; i++) {
    //     array[i] = malloc (MAX_STRING + 1);
    //     if (!array[i]) {
    //     free (array);
    //     return NULL;
    //     }
    // }
    // strncpy (array[0], "ABC", MAX_STRING);
    // strncpy (array[1], "123", MAX_STRING);
    // return array; */
}

char *strExt(const char *s1, const char *s2)
{
    char *pointer = malloc(strlen(s1) + strlen(s2) + 1);

    if (pointer == NULL)
    {
        perror("failed to allocate memory");
        exit(EXIT_FAILURE);
    } 

    return strcat(strcpy(pointer, s1), s2);
}

void *safeMalloc(long size) {
	void *ptr = malloc(size);
	if (ptr == NULL) {
		printf("Fatal Error: Failed allocating memory.");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

char* stringsConcat(char *string1, char* string2) {
    char* concattedString = (char*) safeMalloc(strlen(string1) + strlen(string2) + 1);
	strcpy(concattedString, string1);
	strcat(concattedString, string2);
	return concattedString;
}

int openFileSafe(FILE** fileStream, char* fileName, char* fileExt, char* openMethod) {
	char* fileNameWithExt = stringsConcat(fileName, fileExt);
	*fileStream = fopen(fileNameWithExt, openMethod);
	if(fileStream == NULL) {
		free(fileNameWithExt);
		printf("Error: Unable to read \"%s\". skipping it.\n", fileNameWithExt);
		return FALSE;
	} else {
		return TRUE;
	}
}

void print_error_message(int line, char* message, ...) {
	va_list args; 

	printf("ASSEMBLER ERROR [line %d]: ", line);

	va_start(args, message);
	vprintf(message, args);
	va_end(args);
	printf("\n");
}

int is_integer(char *string) {
	if (atoi(string))
		return TRUE;
	return FALSE;	
}

int is_index(char* operand, int line) {
	char symbol[MAX_LINE_LENGTH];
	int j;

	for (j = 0; operand[j] && operand[j] != '[' && operand[j] != '\0'; j++) {
		symbol[j] = operand[j];
	}

    symbol[j] = '\0';
	if (!validate_symbol_name(symbol, line))
		return FALSE;

	if (operand[j] == '[' && operand[j+1] == 'r' && atoi(&operand[j+2]) <= 9 && operand[j+3] == ']' && operand[j+4] == '\0') /* SYMBOL[rx] */
		return TRUE;

	if (operand[j] == '[' && operand[j+1] == 'r' && atoi(&operand[j+2]) >= 10 && atoi(&operand[j+2]) <= 15 && operand[j+4] == ']' && operand[j+5] == '\0') /* SYMBOL[r1x] */
		return TRUE;	

	return FALSE;
}


int fetch_symbol(int line, char* lineContent, char *symbol_dest) {
	int j, i;
	i = j = 0;

	FIND_NEXT_CHAR(lineContent, i);

	for (; lineContent[i] && lineContent[i] != ':' && lineContent[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
		symbol_dest[j] = lineContent[i];
	}
	symbol_dest[j] = '\0';

	if (lineContent[i] == ':') {
		if (!validate_symbol_name(symbol_dest, line)) {
			print_error_message(line, "invalid symbol name");
			symbol_dest[0] = '\0';
			return TRUE; 
		}
		return FALSE;
	}
	symbol_dest[0] = '\0';
	return FALSE;
}

int fetch_operands(int line, char* lineContent, int i, char **operands_array, int *num_of_operands) {
	int j;
	operands_array[0] = operands_array[1] = NULL;

	if (lineContent[i] == ',') {
		print_error_message(line, "Unexpected comma");
		return FALSE;
	}


	for (*num_of_operands = 0; lineContent[i] != EOF && lineContent[i] != '\n' && lineContent[i];) {
		if (*num_of_operands == 2) {
			print_error_message(line, "Too many operands for this assembly command");
			free(operands_array[0]);
			free(operands_array[1]);
			return FALSE; 
		}

		operands_array[*num_of_operands] = malloc(MAX_LINE_LENGTH);
		for (j = 0; lineContent[i] && lineContent[i] != '\t' && lineContent[i] != ' ' && lineContent[i] != '\n' && lineContent[i] != EOF &&
		            lineContent[i] != ','; i++, j++) {
			operands_array[*num_of_operands][j] = lineContent[i];
		}
		operands_array[*num_of_operands][j] = '\0';
		(*num_of_operands)++;
		FIND_NEXT_CHAR(lineContent, i);

		if (lineContent[i] == '\n' || lineContent[i] == EOF || !lineContent[i]) break;
		else if (lineContent[i] != ',') {
			print_error_message(line, "There needs to be a comma between operands");
			free(operands_array[0]);
			if (*num_of_operands > 1) {
				free(operands_array[1]);
			}
			return FALSE;
		}
		i++;
		FIND_NEXT_CHAR(lineContent, i);
		if (lineContent[i] == '\n' || lineContent[i] == EOF || !lineContent[i])
			print_error_message(line, "There needs to be an operand after comma");
		else if (lineContent[i] == ',') 
			print_error_message(line, "Multiple consecutive commas.");
		else 
			continue;
		{
			free(operands_array[0]);
			if (*num_of_operands > 1) {
				free(operands_array[1]);
			}
			return FALSE;
		}
	}
	return TRUE;
}

int fetch_type(char *lineContent, int *i) {

	char temp[MAX_LINE_LENGTH];
	int j;

	if (lineContent[*i] != '.') {
        return CODE;
    }

	for (j = 0; lineContent[*i] && lineContent[*i] != '\t' && lineContent[*i] != ' '; (*i)++, j++) {
		temp[j] = lineContent[*i];
	}
    temp[j] = '\0';
	if (strcmp(temp, ".data") == 0) {
        return DATA;
    }
    if (strcmp(temp, ".string") == 0) {
        return STRING;
    }
    if (strcmp(temp, ".entry") == 0) {
        return ENTRY;
    }
    if (strcmp(temp, ".extern") == 0) {
        return EXTERN;
    }
	return FAILED_TO_FIND;
}

int fetch_address_type(char *operand, int line) {
	if (operand[0] == '\0')
		return NO_ADDRESS;
	if (operand[0] == 'r' && isdigit(operand[1]) && operand[2] == '\0')
		return REGISTER;
	if (operand[0] == 'r' && atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0')
		return REGISTER;
	if (operand[0] == '#' && is_integer(operand + 1))
		return IMMEDIATE;
	if (operand[0] && is_index(operand, line))
		return INDEX;
	if (validate_symbol_name(operand, line))
		return DIRECT;

	return NO_ADDRESS;
}

unsigned int fetch_register(char *operand) {
	unsigned int reg;
	int index=0, j;
	char temp[MAX_LINE_LENGTH];

	if (operand[0] == 'r' && isdigit(operand[1]) && operand[2] == '\0') {
		reg = atoi(&operand[1]);
		if (reg >= 0 && reg <= 9)
			return reg;
	}

	if (operand[0] == 'r' && atoi(&operand[1]) >= 10 && atoi(&operand[1]) <= 15 && operand[3] == '\0') {
		reg = atoi(&operand[1]);
		return reg;
	}

	INCREASE_I_UNTILL_CHAR(operand, '[', index);
	index++;
	for (j=0; operand[index] && operand[index] != ']'; index++, j++) {
            temp[j] = operand[index];
        }
	temp[j] = '\0';

	return atoi(temp);
}

code_word *generate_first_code_word(assembly_structure *opcode_data) {
	code_word *res_word;
	res_word = (code_word *) malloc(sizeof(code_word));
	res_word->ARE = 4;
	res_word->L = 1;
	res_word->source_address = res_word->source_reg = res_word->destination_address = res_word->destination_reg = res_word->opcode = res_word->funct = 0;
	res_word->opcode = opcode_data->opcode;
	return res_word;
}

code_word *generate_second_code_word(int line, char* lineContent, assembly_structure *opcode_data, char **operands_array, int num_of_operands) {
	code_word *res_word;
	int address1 = NO_ADDRESS;
	int address2 = NO_ADDRESS;

	res_word = (code_word *) malloc(sizeof(code_word));
	res_word->source_address = res_word->source_reg = res_word->destination_address = res_word->destination_reg = res_word->opcode = 0;
	res_word->L = 1;
	res_word->line = line;
	res_word->ARE = 4;
	res_word->funct = opcode_data->funct;

	if ( num_of_operands > 0) {
		address1 = fetch_address_type(operands_array[0], line);
	}
	if ( num_of_operands > 1) {
		address2 = fetch_address_type(operands_array[1], line);
	}

	/* validate the operands against the given function */
	if (!validate_operands(line, address1, address2, num_of_operands, opcode_data)) {
		return NULL;
	}
	

	if (opcode_data->num_of_operands_per_function == 2) {
		res_word->first_operand = operands_array[0];
		res_word->second_operand = operands_array[1];
		res_word->source_address = address1;
		res_word->destination_address = address2;
		if ( address1 == REGISTER) {
			res_word->source_reg = fetch_register(operands_array[0]);
		}
		if ( address2 == REGISTER) {
			res_word->destination_reg = fetch_register(operands_array[1]);
		}
		if ( address1 == IMMEDIATE) {
			res_word->L += 1;
		}
		if ( address2 == IMMEDIATE) {
			res_word->L += 1;
		}
		if ( address1 == DIRECT ) {
			res_word->L += 2;
		}
		if (address1 == INDEX) {
			res_word->L += 2;
			res_word->source_reg = fetch_register(operands_array[0]);
		}
		if ( address2 == DIRECT ) {
			res_word->L += 2;
		}
		if (address2 == INDEX) {
			res_word->L += 2;
			res_word->destination_reg = fetch_register(operands_array[1]);
		}
	}
	else if (opcode_data->num_of_operands_per_function == 1) {
		res_word->first_operand = operands_array[0];
		res_word->destination_address = address1;
		if ( address1 == REGISTER) {
			res_word->destination_reg = fetch_register(operands_array[0]);
		}
		if ( address1 == IMMEDIATE) {
			res_word->L += 1;
		}
		if ( address1 == DIRECT) {
			res_word->L += 2;
		}
		if  (address1 == INDEX) {
			res_word->L += 2;
			res_word->destination_reg = fetch_register(operands_array[0]);
		}
	}

	return res_word;
}

int calculate_base(int ic) {
    int i;
    for (i = ic; i > 0; i--)
    {
        if (i % 16 == 0)
        {
            return i;
        }
    }
    return 0;   
}

int calculate_offset(int ic) {
    return ic - calculate_base(ic);
}

int validate_string(int line, char *lineContent, int i) {

	if (lineContent[i] != '"') {
		print_error_message(line, "string type should start with quotes!");
		return FALSE;
	}
	i++;

	for (; lineContent[i] != '"' && lineContent[i] != EOF && lineContent[i] != '\n'; i++);
	if (lineContent[i] == '"')
		return TRUE;

	return TRUE;
}
