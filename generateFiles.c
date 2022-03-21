
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assemblyStructures.h"
#include "utils.h"
#include "symbolTable.h"
#define FOUR_LSB(value) ((value) & 0x000F) /* get 4 last bits from number */

/* function that generates the .ob file */
void generate_ob_file(char *filename) {
	int i;
	FILE *ob_file_ptr;

	int hasFileOpend = openFileSafe(&ob_file_ptr, fileName, ".ob", "w");
	if(!hasFileOpend) {
		return FALSE;
	}

	fprintf(ob_file_ptr, "%d %d", ICF - IC_INIT_VALUE, DCF);

	for (i = 0; i < ICF - IC_INIT_VALUE; i++) {
		/* OPCODE WORD */
		if (machine_code_section[i].opcode > 0) { 
				fprintf(ob_file_ptr, "\n%.4d A4-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				FOUR_LSB(machine_code_section[i].opcode >> 12), 
				FOUR_LSB(machine_code_section[i].opcode >> 8), 
				FOUR_LSB(machine_code_section[i].opcode >> 4), 
				FOUR_LSB(machine_code_section[i].opcode))
			;
		}
		/* SECOND CODE WORD (funct, regs, addresses) */
		else if (machine_code_section[i].L > 0) { 
				fprintf(ob_file_ptr, "\n%.4d A4-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				machine_code_section[i].funct,
				machine_code_section[i].source_reg,
				machine_code_section[i].source_address << 2 | machine_code_section[i].destination_reg >> 2,
				((machine_code_section[i].destination_reg & 3) << 2) | machine_code_section[i].destination_address)
			;
		}
		/* IMMEDIATE ADDRESS TYPE DATA */
		else if (machine_code_section[i].immediate > 0) { 
				fprintf(ob_file_ptr, "\n%.4d A4-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				FOUR_LSB(machine_code_section[i].immediate >> 12), 
				FOUR_LSB(machine_code_section[i].immediate >> 8), 
				FOUR_LSB(machine_code_section[i].immediate >> 4), 
				FOUR_LSB(machine_code_section[i].immediate))
			;
		}
		/* BASE + OFFSET (DIRECT & INDEX) */
		else {  /* BASE */
				fprintf(ob_file_ptr, "\n%.4d A%x-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				machine_code_section[i].ARE,
				FOUR_LSB(machine_code_section[i].base >> 12), 
				FOUR_LSB(machine_code_section[i].base >> 8), 
				FOUR_LSB(machine_code_section[i].base >> 4), 
				FOUR_LSB(machine_code_section[i].base))
			;
				i++;
				/* OFFSET */
				fprintf(ob_file_ptr, "\n%.4d A%x-B%x-C%x-D%x-E%x",
				i+IC_INIT_VALUE,
				machine_code_section[i].ARE,
				FOUR_LSB(machine_code_section[i].offset >> 12), 
				FOUR_LSB(machine_code_section[i].offset >> 8), 
				FOUR_LSB(machine_code_section[i].offset >> 4), 
				FOUR_LSB(machine_code_section[i].offset))
			;	
		}
		
	}
	/* ALL DATA SECTION */
	for (i = 0; i < DCF; i++) {
        fprintf(ob_file_ptr, "\n%.4d A4-B%x-C%x-D%x-E%x",
			ICF + i,
			FOUR_LSB(machine_data_section[i].data >> 12), 
			FOUR_LSB(machine_data_section[i].data >> 8), 
			FOUR_LSB(machine_data_section[i].data >> 4), 
			FOUR_LSB(machine_data_section[i].data))
		;
	}
    
	fclose(ob_file_ptr);
}

/* function that generates the .ent file */
void generate_ent_file(char *filename, symbolTable* table) {
	FILE *ent_file_ptr;
	int counter = 0;
	symbolItem *temp = table->symbolHead;

	while (temp)
    {
        if (temp->symbolType == DATAANDENTRY || temp->symbolType == DATAANDENTRY)
            counter++;
		temp = temp->next;	
    }
	
	if (counter == 0)
		return;

	temp = table->symbolHead;

	int hasFileOpend = openFileSafe(&ent_file_ptr, fileName, ".ent", "w");
	if(!hasFileOpend) {
		return FALSE;
	}
    
    while (temp)
    {
        if (temp->symbolType == DATAANDENTRY || temp->symbolType == CODEANDENTRY)
            fprintf(ent_file_ptr, "%s,%d,%d\n", temp->symbol, temp->base, temp->offset);
		temp = temp->next;
    }
    
	fclose(ent_file_ptr);
}

/* function that generates the .ext file */
void generate_ext_file(char *filename, symbolTable* table) {
	FILE *ext_file_ptr;
	int counter = 0;
	int i;
	symbolItem *temp = table->symbolHead;

	while (temp)
    {
        if (temp->symbolType == EXTERN)
            counter++;
		temp = temp->next;	
    }
	
	if (counter == 0)
		return;

	temp = table->symbolHead;

	int hasFileOpend = openFileSafe(&ext_file_ptr, fileName, ".ext", "w");
	if(!hasFileOpend) {
		return FALSE;
	}
    
	while (temp)
    {
        if (temp->symbolType == EXTERN) {
			for (i = 0; i < ICF - IC_INIT_VALUE; i++)
			{
				if ( machine_code_section[i].ARE == 1)
				{
					if (strcmp(temp->symbol,machine_code_section[i].first_operand) == 0) {
						fprintf(ext_file_ptr, "%s BASE %d\n", temp->symbol, i+IC_INIT_VALUE);
						fprintf(ext_file_ptr, "%s OFFSET %d\n\n", temp->symbol, i+IC_INIT_VALUE+1);
						i++;
					}
				}
			}
		}
		temp = temp->next;
    }

	fclose(ext_file_ptr);
}

/* function that trigger all of the generate files functions */
void generateFiles(char *filename, symbolTable* table, int* ICF, int* DCF) {
	if (ICF - IC_INIT_VALUE + DCF > MAX_MACHINE_CODE_SECTION){
		printf("The maximum machine code words is %d \n", MAX_MACHINE_CODE_SECTION);
	}else{
		generate_ob_file(filename);
		generate_ent_file(filename, table);
		generate_ext_file(filename, table);
	}
}