#ifndef _FIRST_PHASE_H
#define _FIRST_PHASE_H
#include "symbolTable.h"
#include "optCodeData.h"
#include "consts.h"
#include "utils.h"

int runFirstPass(
	FILE* fileAfterMacroParsing, symbolTable* table, int* IC, int* DC, 
	codeInstruction codeInstructionsList[MAX_MACHINE_CODE_SECTION],
	dataInstruction dataInstructionsList[MAX_MACHINE_DATA_SECTION]
);
#endif
