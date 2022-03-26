# Basic compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBAL_DEPS = consts.h globalStructs.h utils.h  # Dependencies for everything
EXE_DEPS = assembler.o macro.o firstPass.o secondPass.o generateOutputFiles.o macroStructs.o utils.o symbolTable.o optCodeData.o  # Deps for exe

## Executable
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

## Main:
assembler.o: assembler.c $(GLOBAL_DEPS)
	$(CC) -c -g assembler.c $(CFLAGS) -o $@

firstPass.o: firstPass.c firstPass.h $(GLOBAL_DEPS)
	$(CC) -c -g firstPass.c $(CFLAGS) -o $@

secondPass.o: secondPass.c secondPass.h $(GLOBAL_DEPS)
	$(CC) -c -g secondPass.c $(CFLAGS) -o $@

generateOutputFiles.o: generateOutputFiles.c generateOutputFiles.h $(GLOBAL_DEPS)
	$(CC) -c -g generateOutputFiles.c $(CFLAGS) -o $@

macro.o: macro.c macro.h $(GLOBAL_DEPS)
	$(CC) -c -g macro.c $(CFLAGS) -o $@

macroStructs.o: macroStructs.c macroStructs.h $(GLOBAL_DEPS)
	$(CC) -c -g macroStructs.c $(CFLAGS) -o $@

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	$(CC) -c -g utils.c $(CFLAGS) -o $@

symbolTable.o: symbolTable.c symbolTable.h $(GLOBAL_DEPS)
	$(CC) -c -g symbolTable.c $(CFLAGS) -o $@

optCodeData.o: optCodeData.c optCodeData.h $(GLOBAL_DEPS)
	$(CC) -c -g optCodeData.c $(CFLAGS) -o $@

# Clean Target (remove leftovers)
clean:
	rm -rf *.o assembler *.ob *.ent *.ext *.am
