#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdint.h"
#include "symbolTable.h"
#include "utils.h"
#include "optCodeData.h"


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

int isSymbolExistsInTable(char* symbolName, symbolTable* table)
{
    symbolItem *temp = table->symbolHead;
    while (temp)
    {
        if (IS_STR_EQL(temp->symbol, symbolName))
            return TRUE;
        else
            temp = temp->next;
    }
    return FALSE;
}

symbolItem *symbolItemInTable(char* symbolName, symbolTable* table)
{
    symbolItem *temp = table->symbolHead;
    while (temp)
    {
        if (IS_STR_EQL(temp->symbol, symbolName))
            return temp;
        else
            temp = temp->next;
    }
    return NULL;
}

symbolItem * getSymbolItemFromSymbolTable(char*  symbol, symbolTable* table) {
    symbolItem * sl = table->symbolHead;
    while (sl) {
        if (IS_STR_EQL(sl->symbol, symbol)) {
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

void symbolTableAppend(char* symbolName, int symbolType, symbolTable* table, int IC, int DC)
{
	symbolItem* new_symbol = (symbolItem*)safeMalloc(sizeof(symbolItem));
	strcpy(new_symbol->symbol, symbolName);

	new_symbol->value = 0;
	new_symbol->base = 0;
	new_symbol->offset = 0;
	if (symbolType == CODE) {
		new_symbol->value = IC;
		new_symbol->base = calcIcBase(IC);
		new_symbol->offset = calcIcOffset(IC);
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

int isAlphanumericStr(char* string) {
	int i;
	/*check for every char in string if it is non alphanumeric char if it is function returns TRUE*/
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])) return FALSE;
	}
	return TRUE;
}

int isSymbolNameValid(char* name, int line) {
	/* Check length, first char is alpha and all the others are alphanumeric, and not saved word */
	return name[0] && strlen(name) <= MAX_SYMBOL_SIZE && isalpha(name[0]) && isAlphanumericStr(name + 1) &&
	       !isReservedWord(name);
}

void updateSymbolTableDataTypes(symbolTable* table, int IC) {
	
	symbolItem *temp = table->symbolHead;
    while (temp)
    {
        if (temp->symbolType == DATA) {
			temp->value += IC;
			temp->base = calcIcBase(temp->value);
			temp->offset = calcIcOffset(temp->value);
		}
		temp = temp->next;
    }
	
}

int updateSymbolWithEntryAttribute(char* symbolName, int line, symbolTable* table)
{
    symbolItem *temp = table->symbolHead;
    while (temp)
    {
        if (IS_STR_EQL(temp->symbol, symbolName)) {
			if (temp->symbolType == DATA) {
				temp->symbolType = DATA_AND_ENTRY;
            	return TRUE;
			}
			if (temp->symbolType == CODE) {
				temp->symbolType = CODE_AND_ENTRY;
            	return TRUE;
			}
			else {
				printLineError(line, "A symbol can be either entry or extern but not both");
				return FALSE;
			}	
		}
		temp = temp->next;
    }
	
	printLineError(line, "Could not find a symbol in symbol table for this entry");
    return FALSE;
}
