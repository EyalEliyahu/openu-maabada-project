
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


char** getLineContent(char* lineString){
    return NULL;
    /*// TBD
    // int i = 0;
    // char **array = malloc (sizeof (char *) * NUMBER);
    // if (!array)
    //     return NULL;
    // for (i = 0; i < NUMBER; i++) {
    //     array[i] = malloc (MAX_STRING + 1);
    //     if (!array[i]) {
    //     free (array);
    //     return NULL;
    //     }
    // }
    // strncpy (array[0], "ABC", MAX_STRING);
    // strncpy (array[1], "123", MAX_STRING);
    // return array; */
}

void *safeMalloc(long size) {
	void *ptr = malloc(size);
	if (ptr == NULL) {
		printf("Fatal Error: Failed allocating memory.");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

char* stringsConcat(char *string1, char* string2) {
    char* concattedString = (char*) safeMalloc(strlen(string1) + strlen(string2) + 1);
	strcpy(concattedString, string1);
	strcat(concattedString, string2);
	return concattedString;
}