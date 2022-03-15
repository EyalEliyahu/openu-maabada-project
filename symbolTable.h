#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#define MAX_SYMBOL_SIZE 31


typedef struct symbol_line {
	char symbol[MAX_SYMBOL_SIZE];
	int value;
	int base;
    int offset;
	int symbol_type; /* from enum: CODE, DATA, CODE */
	struct symbol_line *next;
} symbol_line;

typedef struct symbolTable {
	symbol_line* symbolHead;
	symbol_line* symbolTail;
} symbolTable;

int symbol_exists_in_table(char *symbol_name, symbolTable* table);
symbol_line *symbol_line_in_table(char *symbol_name, symbolTable* table);
void symbol_table_append(char* symbol_name, int symbol_type, symbolTable* table);
void print_symbol_table(symbolTable* table);
void free_symbol_table(symbolTable* table);
int validate_symbol_name(char *name, int line);
void update_symbol_table_data_types(symbolTable* table);
int update_symbol_with_entry_attribute(char *symbol_name, int line, symbolTable* table);
symbolTable* initSymbolTable();

#endif

