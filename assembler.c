#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "firstPhase.h"
#include "secondPhase.h"
#include "macro.h"
#include "symbolTable.h"
#include "generateOutputFiles.h"

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
	FILE* assemblyFile = NULL;
	FILE* fileAfterMacroParsing = NULL;
	symbolTable* table = NULL;

	if(!openFileSafe(&assemblyFile, fileName, ".as", "r")) {
		return;
	}

	isMacroParseSuccess = translateMacros(assemblyFile, fileName); 
	if(!isMacroParseSuccess)
		return;

	/* Reading the new assembly file - after the macros parsing */
	if(!openFileSafe(&fileAfterMacroParsing, fileName, ".am", "r")) 
		return;

	/* Running First Phase */
	table = initSymbolTable();
	printf("Running First Phase on: \"%s.am\" \n", fileName);

	isFirstPhaseSuccess = runFirstPhase(fileAfterMacroParsing, table, &IC, &DC);
	if(!isFirstPhaseSuccess) 
		return;

	/* move the .am file back to the begining for second phase and start from first line */
	rewind(fileAfterMacroParsing);
	printf("Running Second Phase on: \"%s.am\" \n", fileName);
	isSecondPhaseSuccess = runSecondPhase(fileAfterMacroParsing, table, IC, DC);
	if(!isSecondPhaseSuccess)
		return;

	generateOutputFiles(fileName, table, IC, DC);
}
