#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#define MAX_SYMBOL_SIZE 31


/* define symbol item Struct */
typedef struct symbolItem {
	char symbol[MAX_SYMBOL_SIZE];
	int value;
	int base;
    int offset;
	int symbolType; /* from enum: CODE, DATA, CODE */
	struct symbolItem* next;
} symbolItem;

/* define symbol table Struct */
typedef struct symbolTable {
	symbolItem* symbolHead;
	symbolItem* symbolTail;
} symbolTable;

 symbolItem* getSymbolItem(char* symbolName, symbolTable* table);
void symbolTableAppend(char* symbolName, int symbolType, symbolTable* table, int IC, int DC);
void freeSymbolTable(symbolTable* table);
int isSymbolNameValid(char* name, int line);
void updateSymbolTableDataTypes(symbolTable* table, int IC);
int updateSymbolWithEntryAttribute(char* symbolName, int line, symbolTable* table);
symbolTable* initSymbolTable();

#endif
