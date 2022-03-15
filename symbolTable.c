#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdint.h"
#include "symbolTable.h"
#include "utils.h"
#include "assemblyStructures.h"


void free_symbol_table(symbolTable* table)
{
    symbol_line *temp;

    while (table->symbolHead)
    {
        temp = table->symbolHead->next;
        free(table->symbolHead);
        table->symbolHead = temp;
    }
	free(table);
}

int symbol_exists_in_table(char *symbol_name, symbolTable* table)
{
    symbol_line *temp = table->symbolHead;
    while (temp)
    {
        if (strcmp(temp->symbol, symbol_name) == 0)
            return TRUE;
        else
            temp = temp->next;
    }
    return FALSE;
}

symbol_line *symbol_line_in_table(char *symbol_name, symbolTable* table)
{
    symbol_line *temp = table->symbolHead;
    while (temp)
    {
        if (strcmp(temp->symbol, symbol_name) == 0)
            return temp;
        else
            temp = temp->next;
    }
    return NULL;
}

void print_symbol_table(symbolTable* table) /* Used for debugging */
{
    symbol_line *temp = table->symbolHead;
	printf("----------------------- SYMBOL TABLE START ---------------------\n");
    while (temp)
    {
		printf("Symbol: %s | Value: %d | Base: %d | Offset: %d | Attribute: %d\n", temp->symbol, temp->value, temp->base, temp->offset, temp->symbol_type);
		temp = temp->next;
    }
	printf("----------------------- SYMBOL TABLE END ---------------------\n");
}

symbol_line * get_symbol_line_from_symbol_table(char * symbol, symbolTable* table) {
    symbol_line * sl = table->symbolHead;
    while (sl) {
        if (strcmp(sl->symbol, symbol) == 0) {
            return sl;
        }
        sl = sl->next;
    }
	return NULL;
}

symbolTable* initSymbolTable() {
	symbolTable* newTable = (symbolTable*)malloc(sizeof(symbolTable));
	return newTable;
}

void symbol_table_append(char* symbol_name, int symbol_type, symbolTable* table)
{
	symbol_line* new_symbol = (symbol_line*)malloc(sizeof(symbol_line));

	if (!new_symbol) {
		fprintf(stderr, "Memory allocation for new symbol failed!");
		exit(1);
	}
	
	strcpy(new_symbol->symbol, symbol_name);

	new_symbol->value = 0;
	new_symbol->base = 0;
	new_symbol->offset = 0;
	if (symbol_type == CODE) {
		new_symbol->value = IC;
		new_symbol->base = calculate_base(IC);
		new_symbol->offset = calculate_offset(IC);
	}
	else if (symbol_type == DATA) {
		new_symbol->value = DC;
	}
	new_symbol->symbol_type = symbol_type; 

	if(!table->symbolHead) {
		table->symbolHead = new_symbol;
		table->symbolTail = table->symbolHead;
		}
	else {
		table->symbolTail->next = new_symbol;
		table->symbolTail = table->symbolTail->next;
	}
	table->symbolTail->next = NULL;

	return;
}

int is_alphanumeric_str(char *string) {
	int i;
	/*check for every char in string if it is non alphanumeric char if it is function returns TRUE*/
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])) return FALSE;
	}
	return TRUE;
}

int validate_symbol_name(char *name, int line) {
	/* Check length, first char is alpha and all the others are alphanumeric, and not saved word */
	return name[0] && strlen(name) <= MAX_SYMBOL_SIZE && isalpha(name[0]) && is_alphanumeric_str(name + 1) &&
	       !is_reserved_word(name, line);
}

void update_symbol_table_data_types(symbolTable* table) {
	
	symbol_line *temp = table->symbolHead;
    while (temp)
    {
        if (temp->symbol_type == DATA) {
			temp->value += ICF;
			temp->base = calculate_base(temp->value);
			temp->offset = calculate_offset(temp->value);
		}
		temp = temp->next;
    }
	
}

int update_symbol_with_entry_attribute(char *symbol_name, int line, symbolTable* table)
{
    symbol_line *temp = table->symbolHead;
    while (temp)
    {
        if (strcmp(temp->symbol, symbol_name) == 0) {
			if (temp->symbol_type == DATA) {
				temp->symbol_type = DATAANDENTRY;
            	return TRUE;
			}
			if (temp->symbol_type == CODE) {
				temp->symbol_type = CODEANDENTRY;
            	return TRUE;
			}
			else {
				print_error_message(line, "A symbol can be either entry or extern but not both");
				return FALSE;
			}	
		}
		temp = temp->next;
    }
	
	print_error_message(line, "Could not find a symbol in symbol table for this entry");
    return FALSE;
}