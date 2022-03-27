#ifndef _FIRST_PHASE_H
#define _FIRST_PHASE_H
#include "symbolTable.h"
#include "optCodeData.h"
#include "consts.h"
#include "utils.h"

int runFirstPass(
	FILE* fileAfterMacroParsing, symbolTable* table, int* IC, int* DC, 
	codeInstruction codeInstructionsList[MAX_CODE_INSTRUCTIONS_AMOUNT],
	dataInstruction dataInstructionsList[MAX_DATA_INSTRUCTIONS_AMOUNT]
);
#endif
