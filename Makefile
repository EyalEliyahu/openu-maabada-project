# Basic compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBAL_DEPS = consts.h globalStructs.h  # Dependencies for everything
EXE_DEPS = assembler.o firstPhase.o utils.o symbolTable.o assemblyStructures.o  # Deps for exe

## Executable
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

## Main:
assembler.o: assembler.c $(GLOBAL_DEPS)
	$(CC) -c assembler.c $(CFLAGS) -o $@

firstPhase.o: firstPhase.c firstPhase.h $(GLOBAL_DEPS)
	$(CC) -c firstPhase.c $(CFLAGS) -o $@

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	$(CC) -c utils.c $(CFLAGS) -o $@

symbolTable.o: symbolTable.c symbolTable.h $(GLOBAL_DEPS)
	$(CC) -c symbolTable.c $(CFLAGS) -o $@

assemblyStructures.o: assemblyStructures.c assemblyStructures.h $(GLOBAL_DEPS)
	$(CC) -c assemblyStructures.c $(CFLAGS) -o $@

# Clean Target (remove leftovers)
clean:
	rm -rf *.o assembler *.ob *.ent *.ext *.am