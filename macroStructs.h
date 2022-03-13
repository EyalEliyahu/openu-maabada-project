#include "utils.h"
#include "consts.h"
#include "assembly_structures.h"
#ifndef _MACROSTRUCTS_H
#define _MACROSTRUCTS_H


typedef struct macro_line {
	char macro[MAX_SYMBOL_SIZE];
	int num_of_content_lines;
	char** content_lines;
	struct macro_line *next;
} macro_line;

int macro_exists_in_list(char *macro_name);
void macro_list_append(char* macro_name);
void print_macro_list();
void free_macro_list();
macro_line *macro_line_in_list(char *macro_name);

extern macro_line *macro_head;
extern macro_line *macro_tail;

#endif