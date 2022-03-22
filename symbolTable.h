#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#define MAX_SYMBOL_SIZE 31


typedef struct symbolItem {
	char symbol[MAX_SYMBOL_SIZE];
	int value;
	int base;
    int offset;
	int symbolType; /* from enum: CODE, DATA, CODE */
	struct symbolItem *next;
} symbolItem;

typedef struct symbolTable {
	symbolItem* symbolHead;
	symbolItem* symbolTail;
} symbolTable;

int isSymbolExistsInTable(char *symbolName, symbolTable* table);
symbolItem *symbolItemInTable(char *symbolName, symbolTable* table);
void symbolTableAppend(char* symbolName, int symbolType, symbolTable* table, int IC, int DC);
void printSymbolTable(symbolTable* table);
void freeSymbolTable(symbolTable* table);
int isSymbolNameValid(char *name, int line);
void updateSymbolTableDataTypes(symbolTable* table, int IC);
int updateSymbolWithEntryAttribute(char *symbolName, int line, symbolTable* table);
symbolItem * getSymbolItemFromSymbolTable(char * symbol, symbolTable* table);
symbolTable* initSymbolTable();

#endif

