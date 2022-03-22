# Basic compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBAL_DEPS = consts.h globalStructs.h utils.h  # Dependencies for everything
EXE_DEPS = assembler.o macro.o firstPhase.o secondPhase.o generateOutputFiles.o macroStructs.o utils.o symbolTable.o assemblyStructures.o  # Deps for exe

## Executable
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

## Main:
assembler.o: assembler.c $(GLOBAL_DEPS)
	$(CC) -c -g assembler.c $(CFLAGS) -o $@

firstPhase.o: firstPhase.c firstPhase.h $(GLOBAL_DEPS)
	$(CC) -c -g firstPhase.c $(CFLAGS) -o $@

secondPhase.o: secondPhase.c secondPhase.h $(GLOBAL_DEPS)
	$(CC) -c -g secondPhase.c $(CFLAGS) -o $@

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

assemblyStructures.o: assemblyStructures.c assemblyStructures.h $(GLOBAL_DEPS)
	$(CC) -c -g assemblyStructures.c $(CFLAGS) -o $@

# Clean Target (remove leftovers)
clean:
	rm -rf *.o assembler *.ob *.ent *.ext *.am