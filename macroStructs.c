#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "macroStructs.h"

macro_line *macro_head = NULL;
macro_line *macro_tail = NULL;

macro_line *macro_line_in_list(char *macro_name)
{
    macro_line *temp = macro_head;
    while (temp)
    {
        if (strcmp(temp->macro, macro_name) == 0)
            return temp;
        else
            temp = temp->next;
    }
    return NULL;
}

void print_macro_list() /* Used for debugging */
{
    int i = 0;
    macro_line *temp = macro_head;
    printf("----------------------- MACRO list START ---------------------\n");
    while (temp)
    {
		printf("Macro: %s\n", temp->macro);
        printf("Content:\n");
        while (i < temp->num_of_content_lines)
		{
			printf("\t%s", temp->content_lines[i++]);
		}
		temp = temp->next;
        i = 0;
    }
    printf("----------------------- MACRO list END ---------------------\n");
}

void macro_list_append(char* macro_name)
{
	macro_line* new_macro = (macro_line*)malloc(sizeof(macro_line));

	if (!new_macro) {
		fprintf(stderr, "Memory allocation for new macro failed!");
		exit(1);
	}
	
	strcpy(new_macro->macro,macro_name);
    new_macro->num_of_content_lines = 0;
    new_macro->content_lines = malloc(sizeof(char) * MAX_LINE_LENGTH);

	if(!macro_head) {
		macro_head = new_macro;
		macro_tail = macro_head;
		}
	else {
		macro_tail->next = new_macro;
		macro_tail = macro_tail->next;
	}
	macro_tail->next = NULL;

	return;
}

int macro_exists_in_list(char *macro_name)
{
    macro_line *temp = macro_head;
    while (temp)
    {
        if (strcmp(temp->macro, macro_name) == 0)
            return TRUE;
        else
            temp = temp->next;
    }
    return FALSE;
}

void free_macro_list()
{
    macro_line *temp;

    while (macro_head)
    {
        temp = macro_head->next;
        free(macro_head->content_lines);
        free(macro_head);
        macro_head = temp;
    }
}
