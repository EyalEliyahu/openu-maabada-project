#ifndef _GENERATE_OUTPUT_FILES_H
#define _GENERATE_OUTPUT_FILES_H

#include "symbolTable.h"

void generateOutputFiles(
	char* fileName, symbolTable* table, int IC, int DC,
	codeInstruction codeInstructionsList[MAX_MACHINE_CODE_SECTION],
	dataInstruction dataInstructionsList[MAX_MACHINE_DATA_SECTION]
);
#endif
