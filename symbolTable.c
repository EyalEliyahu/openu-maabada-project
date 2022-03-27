#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdint.h"
#include "symbolTable.h"
#include "utils.h"
#include "optCodeData.h"

/* This function return symbol item if exist, else NULL */
symbolItem* getSymbolItem(char* symbolName, symbolTable* table)
{
    symbolItem* currentSymbol = table->symbolHead;
    while (currentSymbol)
    {
        if (IS_STR_EQL(currentSymbol->symbol, symbolName))
            return currentSymbol;
        else
            currentSymbol = currentSymbol->next;
    }
    return NULL;
}

symbolTable* initSymbolTable() {
	symbolTable* newTable = (symbolTable*)safeMalloc(sizeof(symbolTable));
	return newTable;
}

/* This function append symbol item to symbol table*/
void symbolTableAppend(char* symbolName, int symbolType, symbolTable* table, int IC, int DC)
{
	symbolItem* newSymbol = (symbolItem*)safeMalloc(sizeof(symbolItem));
	strcpy(newSymbol->symbol, symbolName);

	newSymbol->value = 0;
	newSymbol->base = 0;
	newSymbol->offset = 0;
	if (symbolType == CODE) {
		newSymbol->value = IC;
		newSymbol->base = calcIcBase(IC);
		newSymbol->offset = calcIcOffset(IC);
	}
	else if (symbolType == DATA) {
		newSymbol->value = DC;
	}
	newSymbol->symbolType = symbolType; 

	if(!table->symbolHead) {
		table->symbolHead = newSymbol;
		table->symbolTail = table->symbolHead;
		}
	else {
		table->symbolTail->next = newSymbol;
		table->symbolTail = table->symbolTail->next;
	}
	table->symbolTail->next = NULL;

	return;
}

/* This function validate symbol name*/
int isSymbolNameValid(char* name, int line) {
	return name[0] && strlen(name) <= MAX_SYMBOL_SIZE && isalpha(name[0]) && isAlphanumericStr(name + 1) &&
	       !isReservedWord(name);
}

/* This function update symbol data types*/
void updateSymbolTableDataTypes(symbolTable* table, int IC) {
	
	symbolItem* currentSymbol = table->symbolHead;
    while (currentSymbol)
    {
        if (currentSymbol->symbolType == DATA) {
			currentSymbol->value += IC;
			currentSymbol->base = calcIcBase(currentSymbol->value);
			currentSymbol->offset = calcIcOffset(currentSymbol->value);
		}
		currentSymbol = currentSymbol->next;
    }
	
}

/* This function update symbol with entry attribute */
int updateSymbolWithEntryAttribute(char* symbolName, int line, symbolTable* table) {
    symbolItem* currentSymbol = table->symbolHead;
    while (currentSymbol)
    {
        if (IS_STR_EQL(currentSymbol->symbol, symbolName)) {
			if (currentSymbol->symbolType == DATA) {
				currentSymbol->symbolType = DATA_AND_ENTRY;
            	return TRUE;
			}
			if (currentSymbol->symbolType == CODE) {
				currentSymbol->symbolType = CODE_AND_ENTRY;
            	return TRUE;
			}
			else {
				printLineError(line, "A symbol can be either entry or extern but not both");
				return FALSE;
			}	
		}
		currentSymbol = currentSymbol->next;
    }
	
	printLineError(line, "Could not find a symbol in symbol table for this entry");
    return FALSE;
}

/* This function free dynamic allocated memory used for symbol table */
void freeSymbolTable(symbolTable* table) {
    symbolItem* currentSymbol;

    while (table->symbolHead)
    {
        currentSymbol = table->symbolHead->next;
        free(table->symbolHead);
        table->symbolHead = currentSymbol;
    }
	free(table);
}
