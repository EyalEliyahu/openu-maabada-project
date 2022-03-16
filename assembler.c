#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "firstPhase.h"
#include "secondPhase.h"
#include "macro.h"
#include "symbolTable.h"

void compileFile(char* fileName);


int main(int argc, char* argv[]) {
	int fileIndex;
	for (fileIndex = 1; fileIndex < argc; ++fileIndex) {
		compileFile(argv[fileIndex]);
	}
	return EXIT_SUCCESS;
}

void compileFile(char* fileName) {
	int isMacroParseSuccess;
	int isFirstPhaseSuccess;
	int isSecondPhaseSuccess;
	int IC = IC_INIT_VALUE;
	int DC = DC_INIT_VALUE;
	int ICF = 0;
	int DCF = 0;

	isMacroParseSuccess = macro_process(fileName); 
	if(isMacroParseSuccess) {
		FILE* fileAfterMacroParsing = NULL;
		symbolTable* table = initSymbolTable();
		/* Reading the new assembly file - after the macros parsing */
		if(openFileSafe(&fileAfterMacroParsing, fileName, ".am", "r")) {
			/* Running First Phase */
			printf("Running First Phase on: \"%s.am\" \n", fileName);
			isFirstPhaseSuccess = runFirstPhase(fileAfterMacroParsing, table, &IC, &DC);
			if(isFirstPhaseSuccess) {
				ICF = IC; /* save the final IC value to new variable */
				DCF = DC; /* save the final DC value to new variable */
				/* move the .am file back to the begining for second phase and start from first line */
				rewind(fileAfterMacroParsing);
				isSecondPhaseSuccess = runSecondPhase(fileAfterMacroParsing, table);

			}
		}
	}
	
}
