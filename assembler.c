#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "firstPass.h"
#include "secondPass.h"
#include "macro.h"
#include "symbolTable.h"
#include "generateOutputFiles.h"

void compileFile(char* fileName);

/* The main function - start process for each file*/
int main(int argc, char* argv[]) {
	int fileIndex;
	for (fileIndex = 1; fileIndex < argc; ++fileIndex) {
		compileFile(argv[fileIndex]);
	}
	return EXIT_SUCCESS;
}

/* Handle the compile's process of file*/
void compileFile(char* fileName) {
	int isMacroParseSuccess;
	int isFirstPassSuccess;
	int isSecondPassSuccess;
	int IC = IC_INIT_VALUE;
	int DC = DC_INIT_VALUE;
	FILE* assemblyFile = NULL;
	FILE* fileAfterMacroParsing = NULL;
	symbolTable* table = NULL;

	printf("------- Starting Assembler on file: %s.as -------\n", fileName);

	if(openFileSafe(&assemblyFile, fileName, ".as", "r")) {
		isMacroParseSuccess = translateMacros(assemblyFile, fileName); 
		if(isMacroParseSuccess) {
			/* Reading the new assembly file - after the macros parsing */
			if(openFileSafe(&fileAfterMacroParsing, fileName, ".am", "r")) {
				/* Running First Pass */
				table = initSymbolTable();
				printf("Running First Pass on: \"%s.am\" \n", fileName);
				isFirstPassSuccess = runFirstPass(fileAfterMacroParsing, table, &IC, &DC);
				if(isFirstPassSuccess)  {
					/* move the .am file back to the begining for second pass and start from first line */
					rewind(fileAfterMacroParsing);
					printf("Running Second Pass on: \"%s.am\" \n", fileName);
					isSecondPassSuccess = runSecondPass(fileAfterMacroParsing, table, IC, DC);
					if(isSecondPassSuccess) {
						generateOutputFiles(fileName, table, IC, DC);
					}

				}

			}

		}
	}
	printf("------- Finish Assembler on file: %s.as -------\n", fileName);
}
