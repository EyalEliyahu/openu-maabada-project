#ifndef _SECOND_PHASE_H
#define _SECOND_PHASE_H

int updateCodeWordByType(int line, int *j, int *i, char* operand, int address_type, symbolTable* table);
int runSecondPhase(FILE* fileAfterMacroParsing, symbolTable* table, int IC);
#endif
