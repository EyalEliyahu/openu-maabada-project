#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdint.h"
#include "symbolTable.h"
#include "utils.h"
#include "assemblyStructures.h"


void freeSymbolTable(symbolTable* table)
{
    symbolItem *temp;

    while (table->symbolHead)
    {
        temp = table->symbolHead->next;
        free(table->symbolHead);
        table->symbolHead = temp;
    }
	free(table);
}

int symbolExistsInTable(char *symbolName, symbolTable* table)
{
    symbolItem *temp = table->symbolHead;
    while (temp)
    {
        if (strcmp(temp->symbol, symbolName) == 0)
            return TRUE;
        else
            temp = temp->next;
    }
    return FALSE;
}

symbolItem *symbolItemInTable(char *symbolName, symbolTable* table)
{
    symbolItem *temp = table->symbolHead;
    while (temp)
    {
        if (strcmp(temp->symbol, symbolName) == 0)
            return temp;
        else
            temp = temp->next;
    }
    return NULL;
}

void printSymbolTable(symbolTable* table) /* Used for debugging */
{
    symbolItem *temp = table->symbolHead;
	printf("----------------------- SYMBOL TABLE START ---------------------\n");
    while (temp)
    {
		printf("Symbol: %s | Value: %d | Base: %d | Offset: %d | Attribute: %d\n", temp->symbol, temp->value, temp->base, temp->offset, temp->symbolType);
		temp = temp->next;
    }
	printf("----------------------- SYMBOL TABLE END ---------------------\n");
}

symbolItem * getSymbolItemFromSymbolTable(char * symbol, symbolTable* table) {
    symbolItem * sl = table->symbolHead;
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

void symbolTableAppend(char* symbolName, int symbolType, symbolTable* table)
{
	symbolItem* new_symbol = (symbolItem*)malloc(sizeof(symbolItem));

	if (!new_symbol) {
		fprintf(stderr, "Memory allocation for new symbol failed!");
		exit(1);
	}
	
	strcpy(new_symbol->symbol, symbolName);

	new_symbol->value = 0;
	new_symbol->base = 0;
	new_symbol->offset = 0;
	if (symbolType == CODE) {
		new_symbol->value = IC;
		new_symbol->base = calculate_base(IC);
		new_symbol->offset = calculate_offset(IC);
	}
	else if (symbolType == DATA) {
		new_symbol->value = DC;
	}
	new_symbol->symbolType = symbolType; 

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

int isAlphanumericStr(char *string) {
	int i;
	/*check for every char in string if it is non alphanumeric char if it is function returns TRUE*/
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])) return FALSE;
	}
	return TRUE;
}

int validateSymbolName(char *name, int line) {
	/* Check length, first char is alpha and all the others are alphanumeric, and not saved word */
	return name[0] && strlen(name) <= MAX_SYMBOL_SIZE && isalpha(name[0]) && isAlphanumericStr(name + 1) &&
	       !is_reserved_word(name, line);
}

void updateSymbolTableDataTypes(symbolTable* table) {
	
	symbolItem *temp = table->symbolHead;
    while (temp)
    {
        if (temp->symbolType == DATA) {
			temp->value += ICF;
			temp->base = calculate_base(temp->value);
			temp->offset = calculate_offset(temp->value);
		}
		temp = temp->next;
    }
	
}

int updateSymbolWithEntryAttribute(char *symbolName, int line, symbolTable* table)
{
    symbolItem *temp = table->symbolHead;
    while (temp)
    {
        if (strcmp(temp->symbol, symbolName) == 0) {
			if (temp->symbolType == DATA) {
				temp->symbolType = DATAANDENTRY;
            	return TRUE;
			}
			if (temp->symbolType == CODE) {
				temp->symbolType = CODEANDENTRY;
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