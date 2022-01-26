#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

/*TBD*/
void compileFile(char* fileName);
/*TBD*/
bool readFile(char* fileName, FILE* fileStream);


int main(int argc, char* argv[]) {
	int fileIndex;
	for (fileIndex = 1; fileIndex < argc; ++fileIndex) {
		compileFile(argv[fileIndex]);
	}
	return EXIT_SUCCESS;
}

void compileFile(char* fileName) {
	FILE* fileStream = NULL;
	if(readFile(fileName, fileStream)) {
		// Remove comments and empty lines
		// interpret macros
		// first run
		//secord run
		//save output filess
	}
}

bool readFile(char* fileName, FILE* fileStream) {
	char* fileNameWithExtention = stringsConcat(fileName, ".as");

	/* Open file, skip on failure */
	fileStream = fopen(fileNameWithExtention, "r");
	if (fileStream == NULL) {
		printf("Error: Unable to read \"%s\". skipping it.\n", fileNameWithExtention);
		free(fileNameWithExtention);
		return false;
	}
	return true;
}