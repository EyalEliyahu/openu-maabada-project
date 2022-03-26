#ifndef _FIRST_PHASE_H
#define _FIRST_PHASE_H
#include "symbolTable.h"
#include "assemblyStructures.h"
#include "consts.h"
#include "utils.h"

int runFirstPass(FILE* fileAfterMacroParsing, symbolTable* table, int* IC, int* DC);

#endif
