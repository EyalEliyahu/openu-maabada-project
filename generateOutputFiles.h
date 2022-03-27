#ifndef _GENERATE_OUTPUT_FILES_H
#define _GENERATE_OUTPUT_FILES_H

#include "symbolTable.h"

void generateOutputFiles(
	char* fileName, symbolTable* table, int IC, int DC,
	codeInstruction codeInstructionsList[MAX_CODE_INSTRUCTIONS_AMOUNT],
	dataInstruction dataInstructionsList[MAX_DATA_INSTRUCTIONS_AMOUNT]
);
#endif
