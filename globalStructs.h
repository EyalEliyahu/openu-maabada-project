#ifndef _GLOBALSTRUCTS_H
#define _GLOBALSTRUCTS_H

#include <stdbool.h>

/**
 * Represents a line in an input assembly file.
 */
typedef struct lineDetails {
	/** line index in the file */
	long index;
	/** File name */
	char *fileName;
	/** Line content */
	char *rawContent;
    /** Strings array of each word/symbol in the file, splited by space charecter*/
    /*char * splittedContent[];*/
} lineDetails;

typedef enum lineType {
	LABEL,
    INSTRUCTION,
    COMMENT,
    EMPTY_LINE
} lineType;

#endif
