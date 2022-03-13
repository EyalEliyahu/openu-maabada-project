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

int symbol_exists_in_table(char *symbol_name);
symbol_line *symbol_line_in_table(char *symbol_name);
void symbol_table_append(char* symbol_name, int symbol_type);
void print_symbol_table();
void free_symbol_table();
int validate_symbol_name(char *name, int line);
void update_symbol_table_data_types();
int update_symbol_with_entry_attribute(char *symbol_name, int line);

extern symbol_line *symbol_head;
extern symbol_line *symbol_tail;

#endif
