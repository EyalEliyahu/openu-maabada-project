#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
/* #include "firstPhase.h"*/
#include "macro.h"

/*TBD*/
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
	/*int isFirstPhaseSuccess;
	int isSecondPhaseSuccess;
*/
	isMacroParseSuccess = macro_process(fileName); 
	if (isMacroParseSuccess)
	{
		printf("OK");
	}
	else
	{
		printf("NOT OK");
	}
	
	
	/*if(isMacroParseSuccess) {
		FILE* fileAfterMacroParsing = NULL;
		 Reading the new assembly file - after the macros parsing */
	/*	if(openFileSafe(fileAfterMacroParsing, fileName, ".am", "r")) {
			 Running First Phase */
	/*		printf("Running First Phase on: \"%s.am\" \n", fileName);
			isFirstPhaseSuccess = runFirstPhase(fileAfterMacroParsing);
		} else {
			isFirstPhaseSuccess = FALSE;
			isSecondPhaseSuccess = FALSE;
		}
	}*/
	
}
