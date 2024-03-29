#ifndef _SECOND_PHASE_H
#define _SECOND_PHASE_H

int updateCodeWordByType(
    int line, int* j, int* i, char* operand, int address_type,
    symbolTable* table,  codeInstruction codeInstructionsList[MAX_CODE_INSTRUCTIONS_AMOUNT]
);
int runSecondPass(
    FILE* fileAfterMacroParsing, symbolTable* table, int IC, int DC, 
    codeInstruction codeInstructionsList[MAX_CODE_INSTRUCTIONS_AMOUNT]
);
#endif
