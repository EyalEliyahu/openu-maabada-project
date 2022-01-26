# Basic compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBAL_DEPS = consts.h globalStructs.h # Dependencies for everything
EXE_DEPS = assembler.o  utils.o  # Deps for exe

## Executable
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

## Main:
assembler.o: assembler.c $(GLOBAL_DEPS)
	$(CC) -c assembler.c $(CFLAGS) -o $@


## Useful functions:
utils.o: utils.c $(GLOBAL_DEPS)
	$(CC) -c utils.c $(CFLAGS) -o $@

# Clean Target (remove leftovers)
clean:
	rm -rf *.o