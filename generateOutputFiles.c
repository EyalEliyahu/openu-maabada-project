
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assemblyStructures.h"
#include "utils.h"
#include "symbolTable.h"



#define MAST_EXCEPT_LAST_FOUR_BITS(value) ((value) & 0x000F) /* get 4 last bits from number */
#define VALUE_TO_MASKED_BITS(value)\
					MAST_EXCEPT_LAST_FOUR_BITS(value >> 12),\
					MAST_EXCEPT_LAST_FOUR_BITS(value >> 8),\
					MAST_EXCEPT_LAST_FOUR_BITS(value >> 4),\
					MAST_EXCEPT_LAST_FOUR_BITS(value)

#define IS_SYMBOL_OF_ENTRY_TYPE(symbolTableItemIterator) (symbolTableItemIterator->symbolType == CODE_AND_ENTRY || symbolTableItemIterator->symbolType == DATA_AND_ENTRY)

/* function that generates the .ob file */
void generateObFile(char *fileName, int IC, int DC) {
	int indexInSection;
	FILE *obFile = NULL;
	int hasFileOpened;
	codeWord currentInstruction;
	char* WORD_FORMAT = "\n%.4d A4-B%x-C%x-D%x-E%x";
	char* WORD_WITH_BASE_FORMAT = "\n%.4d A%x-B%x-C%x-D%x-E%x";
	int instructionAmount = IC - IC_INIT_VALUE;
	hasFileOpened = openFileSafe(&obFile, fileName, ".ob", "w");
	if(!hasFileOpened) {
		return;
	}

	fprintf(obFile, "%d %d", instructionAmount, DC);

	for (indexInSection = 0; indexInSection < instructionAmount; indexInSection++) {
		currentInstruction = machineCodeSection[indexInSection];
		/* OPCODE WORD */
		if (currentInstruction.opcode > 0) { 
				fprintf(
					obFile,
					WORD_FORMAT,
					indexInSection+IC_INIT_VALUE,
					VALUE_TO_MASKED_BITS(currentInstruction.opcode)
				);
			;
		}
		/* SECOND CODE WORD (funct, regs, addresses) */
		else if (currentInstruction.L > 0) { 
				fprintf(
					obFile,
					WORD_FORMAT,
					indexInSection+IC_INIT_VALUE,
					currentInstruction.funct,
					currentInstruction.sourceRegister,
					currentInstruction.sourceAddress << 2 | currentInstruction.destinationRegister >> 2,
					((currentInstruction.destinationRegister & 3) << 2) | currentInstruction.destinationAddress
				);
		}
		/* IMMEDIATE ADDRESS TYPE DATA */
		else if (currentInstruction.immediate > 0) { 
				fprintf(
					obFile,
					WORD_FORMAT,
					indexInSection+IC_INIT_VALUE,
					VALUE_TO_MASKED_BITS(currentInstruction.immediate)
				)
			;
		}
		/* BASE + OFFSET (DIRECT & INDEX) */
		else {  /* BASE */
				fprintf(
					obFile,
					WORD_WITH_BASE_FORMAT,
					indexInSection+IC_INIT_VALUE,
					currentInstruction.ARE,
					VALUE_TO_MASKED_BITS(currentInstruction.base)
				);

				indexInSection++;
				currentInstruction = machineCodeSection[indexInSection];
				/* OFFSET */
				fprintf(
					obFile,
					WORD_WITH_BASE_FORMAT,
					indexInSection+IC_INIT_VALUE,
					currentInstruction.ARE,
					VALUE_TO_MASKED_BITS(currentInstruction.offset)
			);	
		}
		
	}
	/* ALL DATA SECTION */
	for (indexInSection = 0; indexInSection < DC; indexInSection++) {
        fprintf(
			obFile,
			WORD_FORMAT,
			IC + indexInSection,
			VALUE_TO_MASKED_BITS(machineDataSection[indexInSection].data)
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
					if (IS_STR_EQL(temp->symbol, machineCodeSection[i].firstOperand)) {
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
