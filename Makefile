# Basic compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBAL_DEPS = consts.h globalStructs.h  # Dependencies for everything
# EXE_DEPS = assembler.o firstPhase.o macro.o macroStructs.o utils.o symbolTable.o assembly_structures.o  # Deps for exe
EXE_DEPS = assembler.o macro.o macroStructs.o utils.o symbolTable.o assembly_structures.o  # Deps for exe

## Executable
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

## Main:
assembler.o: assembler.c $(GLOBAL_DEPS)
	$(CC) -c -g assembler.c $(CFLAGS) -o $@

# firstPhase.o: firstPhase.c firstPhase.h $(GLOBAL_DEPS)
# 	$(CC) -c -g firstPhase.c $(CFLAGS) -o $@

macro.o: macro.c macro.h $(GLOBAL_DEPS)
	$(CC) -c -g macro.c $(CFLAGS) -o $@

macroStructs.o: macroStructs.c macroStructs.h $(GLOBAL_DEPS)
	$(CC) -c -g macroStructs.c $(CFLAGS) -o $@

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	$(CC) -c -g utils.c $(CFLAGS) -o $@

symbolTable.o: symbolTable.c symbolTable.h $(GLOBAL_DEPS)
	$(CC) -c -g symbolTable.c $(CFLAGS) -o $@

assembly_structures.o: assembly_structures.c assembly_structures.h $(GLOBAL_DEPS)
	$(CC) -c -g assembly_structures.c $(CFLAGS) -o $@

# Clean Target (remove leftovers)
clean:
	rm -rf *.o assembler *.ob *.ent *.ext *.am