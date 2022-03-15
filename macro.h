#ifndef _MACRO_H
#define _MACRO_H

int process_macro_line(int line, char *lineContent, int *in_macro, char *macro, char *file, FILE *am_file_ptr);

int macro_process(char *fileName);

#endif
