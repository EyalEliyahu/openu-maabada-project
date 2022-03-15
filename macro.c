#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "macroStructs.h"

/* This function handles the macro replacement */
int process_macro_line(int line, char *lineContent, int *in_macro, char *macro, char *file, FILE *am_file_ptr) {
	int i=0, j=0, k=0;
	char firstWord[MAX_LINE_LENGTH];
	macro_line *temp;
	char* line_in_macro = "";
	/* look for the next char that is not whitespace/tab/newline */
	FIND_NEXT_CHAR(lineContent, i); 

	/* Get the line content after removing spaces and tabs from the beginning until reach whitespace/tab/newline */
	for (; lineContent[i] && lineContent[i] != '\n' && lineContent[i] != '\t' && lineContent[i] != ' ' && lineContent[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
		firstWord[j] = lineContent[i];
	}
	firstWord[j] = '\0';

	/* Check if the line contains macro that we already saved */
	if(macro_exists_in_list(firstWord) && !*in_macro) {
		temp = macro_line_in_list(firstWord);
		/* replace macro name with macro content */
		while (k < temp->num_of_content_lines)
		{
			fprintf(am_file_ptr, "%s", temp->content_lines[k++]);
		}
		return TRUE;
	}

	i = 0;
	j = 0;
	FIND_NEXT_CHAR(lineContent, i); 
	/* Checks if this line is start of new macro */
    if (strncmp("macro", lineContent+i, 5) == 0 && !*in_macro) {
		/* go to the end of the word: macro */
		INCREASE_I_UNTILL_CHAR(lineContent, 'o', i);
		FIND_NEXT_CHAR(lineContent, i);
		for (; lineContent[i] && lineContent[i] != ' ' && lineContent[i] != '\n' && lineContent[i] != '\t' && lineContent[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
			macro[j] = lineContent[i];
		}
		macro[j] = '\0';

		*in_macro = TRUE;
		macro_list_append(macro);
		return TRUE;
	}

	/* Checks if this line is end of a macro */
	if (strncmp("endm", lineContent+i, 4) == 0 && *in_macro) {
		*in_macro = FALSE;
		i=0;
		while (i < MAX_LINE_LENGTH)
		{
			macro[i] = '\0';
			++i;
		}
		
		return TRUE;
	}

	/* write regular line to am file */
	if (!*in_macro)	
		fprintf(am_file_ptr, "%s", lineContent);
	/* the line is part of a macro so we added it to macro content */
	else {
		temp = macro_line_in_list(macro);
		temp->num_of_content_lines += 1;
		temp->content_lines = realloc(temp->content_lines, temp->num_of_content_lines*sizeof(lineContent));
		line_in_macro = malloc(sizeof(char) * (strlen(lineContent) + 1));
		strcpy(line_in_macro,lineContent);
		temp->content_lines[temp->num_of_content_lines-1] = line_in_macro;
		return TRUE;
	}	

    return TRUE;
}


int macro_process(char *fileName)
{
	char *temp_string = "";
	int macro_phase_success = TRUE;
	char macro[MAX_LINE_LENGTH];
	int in_macro = FALSE;
	char lineContent[MAX_LINE_LENGTH + 2];
	int line = 1;
	FILE *assembly_file_ptr;
	FILE *am_file_ptr;

	
	if(!openFileSafe(&am_file_ptr, fileName, ".am", "w") || openFileSafe(&assembly_file_ptr, fileName, ".as", "r")) {
		return FALSE;
	}

	/* run the macro_process_line function on every line in .as file */
	for (; fgets(lineContent, MAX_LINE_LENGTH + 2, assembly_file_ptr) != NULL; line++)
	{
		/* check if line no reach the max length */ 
		if (!feof(assembly_file_ptr) && strchr(lineContent, '\n') == NULL)
		{
			printErrorMessage(line, "line exceeds the max line length");
			macro_phase_success = FALSE;
			/* if the line is too long continue the rest of the chars to get to the new line */ 
			while (*temp_string != '\n' && *temp_string != EOF)
			{
				*temp_string = fgetc(assembly_file_ptr);
			}
		}
		else
		{
			/* run process_macro_line function */ 
			if (!process_macro_line(line, lineContent, &in_macro, macro, fileName, am_file_ptr)) {
				macro_phase_success = FALSE;
			}
		}
	}
	
	/* free dynamic allocated memory used for macro and close am and assembly files */
	fclose(am_file_ptr);
	fclose(assembly_file_ptr);
	free_macro_list();
	return macro_phase_success;
}
