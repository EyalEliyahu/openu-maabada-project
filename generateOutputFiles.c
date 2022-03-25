
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assemblyStructures.h"
#include "utils.h"
#include "symbolTable.h"

#define FOUR_LSB(value) ((value) & 0x000F) /* get 4 last bits from number */
#define IS_SYMBOL_OF_ENTRY_TYPE(symbolTableItemIterator) (symbolTableItemIterator->symbolType == CODE_AND_ENTRY || symbolTableItemIterator->symbolType == DATA_AND_ENTRY)

/* function that generates the .ob file */
void generateObFile(char *fileName, int IC, int DC) {
	int i;
	FILE *obFile = NULL;
	int hasFileOpened;

	hasFileOpened = openFileSafe(&obFile, fileName, ".ob", "w");
	if(!hasFileOpened) {
		return;
	}

	fprintf(obFile, "%d %d", IC - IC_INIT_VALUE, DC);

	for (i = 0; i < IC - IC_INIT_VALUE; i++) {
		/* OPCODE WORD */
		if (machineCodeSection[i].opcode > 0) { 
				fprintf(obFile, "\n%.4d A4-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				FOUR_LSB(machineCodeSection[i].opcode >> 12), 
				FOUR_LSB(machineCodeSection[i].opcode >> 8), 
				FOUR_LSB(machineCodeSection[i].opcode >> 4), 
				FOUR_LSB(machineCodeSection[i].opcode))
			;
		}
		/* SECOND CODE WORD (funct, regs, addresses) */
		else if (machineCodeSection[i].L > 0) { 
				fprintf(obFile, "\n%.4d A4-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				machineCodeSection[i].funct,
				machineCodeSection[i].sourceRegister,
				machineCodeSection[i].sourceAddress << 2 | machineCodeSection[i].destinationRegister >> 2,
				((machineCodeSection[i].destinationRegister & 3) << 2) | machineCodeSection[i].destinationAddress)
			;
		}
		/* IMMEDIATE ADDRESS TYPE DATA */
		else if (machineCodeSection[i].immediate > 0) { 
				fprintf(obFile, "\n%.4d A4-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				FOUR_LSB(machineCodeSection[i].immediate >> 12), 
				FOUR_LSB(machineCodeSection[i].immediate >> 8), 
				FOUR_LSB(machineCodeSection[i].immediate >> 4), 
				FOUR_LSB(machineCodeSection[i].immediate))
			;
		}
		/* BASE + OFFSET (DIRECT & INDEX) */
		else {  /* BASE */
				fprintf(obFile, "\n%.4d A%x-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				machineCodeSection[i].ARE,
				FOUR_LSB(machineCodeSection[i].base >> 12), 
				FOUR_LSB(machineCodeSection[i].base >> 8), 
				FOUR_LSB(machineCodeSection[i].base >> 4), 
				FOUR_LSB(machineCodeSection[i].base))
			;
				i++;
				/* OFFSET */
				fprintf(obFile, "\n%.4d A%x-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				machineCodeSection[i].ARE,
				FOUR_LSB(machineCodeSection[i].offset >> 12), 
				FOUR_LSB(machineCodeSection[i].offset >> 8), 
				FOUR_LSB(machineCodeSection[i].offset >> 4), 
				FOUR_LSB(machineCodeSection[i].offset))
			;	
		}
		
	}
	/* ALL DATA SECTION */
	for (i = 0; i < DC; i++) {
        fprintf(
			obFile, "\n%.4d A4-B%x-C%x-D%x-E%x",
			IC + i,
			FOUR_LSB(machineDataSection[i].data >> 12), 
			FOUR_LSB(machineDataSection[i].data >> 8), 
			FOUR_LSB(machineDataSection[i].data >> 4), 
			FOUR_LSB(machineDataSection[i].data)
		);
	}
    
	fclose(obFile);
}

/* function that generates the .ent file */
void generateEntFile(char *fileName, symbolTable* table) {
	FILE *entFile;
	int entrySymbolsCount = 0;
	int hasFileOpened;
	symbolItem *symbolTableItemIterator = table->symbolHead;

	while (symbolTableItemIterator)
    {
        if (IS_SYMBOL_OF_ENTRY_TYPE(symbolTableItemIterator))
            entrySymbolsCount++;
		symbolTableItemIterator = symbolTableItemIterator->next;	
    }
	
	if (entrySymbolsCount == 0)
		return;
	
	hasFileOpened = openFileSafe(&entFile, fileName, ".ent", "w");
	if(!hasFileOpened) {
		return;
	}

	symbolTableItemIterator = table->symbolHead;
    while (symbolTableItemIterator)
    {
        if (IS_SYMBOL_OF_ENTRY_TYPE(symbolTableItemIterator))
            fprintf(entFile, "%s,%d,%d\n", symbolTableItemIterator->symbol, symbolTableItemIterator->base, symbolTableItemIterator->offset);
		symbolTableItemIterator = symbolTableItemIterator->next;
    }
    
	fclose(entFile);
}

/* function that generates the .ext file */
void generateExtFile(char *fileName, symbolTable* table, int IC) {
	FILE *extFile = NULL;
	int externSymbolsCount = 0;
	int i, hasFileOpened;
	symbolItem *temp = table->symbolHead;

	while (temp)
    {
        if (temp->symbolType == EXTERN)
            externSymbolsCount++;
		temp = temp->next;	
    }
	
	if (externSymbolsCount == 0)
		return;

		
	hasFileOpened = openFileSafe(&extFile, fileName, ".ext", "w");
	if(!hasFileOpened) {
		return;
	}

    temp = table->symbolHead;
	while (temp)
    {
        if (temp->symbolType == EXTERN) {
			for (i = 0; i < IC - IC_INIT_VALUE; i++)
			{
				if ( machineCodeSection[i].ARE == 1)
				{
					if (strcmp(temp->symbol, machineCodeSection[i].firstOperand) == 0) {
						fprintf(extFile, "%s BASE %d\n", temp->symbol, i+IC_INIT_VALUE);
						fprintf(extFile, "%s OFFSET %d\n\n", temp->symbol, i+IC_INIT_VALUE+1);
						i++;
					}
				}
			}
		}
		temp = temp->next;
    }

	fclose(extFile);
}

/* function that trigger all of the generate files functions */
void generateOutputFiles(char *fileName, symbolTable* table, int IC, int DC) {
	generateObFile(fileName, IC, DC);
	generateEntFile(fileName, table);
	generateExtFile(fileName, table, IC);
}
