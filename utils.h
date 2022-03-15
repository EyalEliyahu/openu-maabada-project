#ifndef _UTILS_H
#define _UTILS_H

#include "consts.h"
#include "globalStructs.h"
#include "symbolTable.h"
#include "assemblyStructures.h"

/*
 * TODO
 */
char** getLineContent(char* lineString);

/*
 * TODO
 */
void *safeMalloc(long size);

/*
 * TODO
 */
char* stringsConcat(char *string1, char* string2);

int openFileSafe(FILE** fileStream, char* fileName, char* fileExt, char* openMethod);

char* strExt(const char *s1, const char *s2);

int fetch_symbol(int line, char* lineContent, char *symbol_dest);



/*#define FIND_NEXT_CHAR(string, index) \
        for (;string[(index)] && (string[(index)] == '\t' || string[(index)] == ' '); (++(index)))\
        ;\
*/

#define FIND_NEXT_CHAR(string, index) \
        while (string[i] == ' ' || string[i] == '\t'|| string[i] == '\n')\
        {\
                ++index;\
        }

#define INCREASE_I_UNTILL_CHAR(string, char, index) \
        for (;string[(index)] != char; (++(index)))\
        ;\
        (++(index));

#define THERE_IS_SYMBOL\
        symbolName[0] != '\0'

void append(char* s, char c);
void print_error_message(int line, char* message, ...);
int fetch_symbol(int line, char* lineContent, char *symbol_dest);
int fetch_type(char *lineContent, int *i);
int fetch_operands(int line, char* lineContent, int i, char **operands_array, int *num_of_operands);
code_word *generate_first_code_word(assembly_structure *opcode_data);
code_word *generate_second_code_word(int line, char* lineContent, assembly_structure *opcode_data, char **operands_array, int num_of_operands);
int calculate_base(int ic);
int calculate_offset(int ic);
int validate_string(int line, char *lineContent, int i);

#endif

