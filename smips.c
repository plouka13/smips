// smips.c: an emulator for small simple subset of the MIPS
// Peter Louka, z5207453
// July-August 2020

#include <stdio.h>
#include <stdlib.h>

#include "operations.h"

#define MAX_CODES 1001

#define N_REGISTERS 32

#define TOP_MASK 0xfc000000
#define BOTTOM_MASK 0x3f

#define S_MASK 0x3e00000
#define T_MASK 0x1f0000
#define D_MASK 0xf800
#define I_MASK 0xffff

#define S_OFFSET 21
#define T_OFFSET 16
#define D_OFFSET 11

int *get_codes (FILE *file);
void decode_print (int codes[], char *filename);
int do_decode_print (int num, int instruction_code);
void decode_exec (int codes[], char *filename, int registers[]);
int do_decode_exec (int num, int instruction_code, int registers[], int *PC);
int get_reg_from_code (int instruction_code, int offset, int mask);
void print_registers(int registers[]);

int main (int argc, char *argv[]) {
	
	char *filename = argv[1];

	// Check args
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <.hex file>\n", argv[0]);
		return 1;
	}

	FILE *hex_file = fopen(filename, "r");
	// Check fopen success
	if (hex_file == NULL) {
		fprintf(stderr, "File %s failed to open!\n", filename);
		return 1;
	}

	// Get hex codes into array
	// codes[0] value is number of codes
	int *codes = get_codes(hex_file);

	fclose(hex_file);

	// Store values of all registers in registers array
	int registers[N_REGISTERS] = {0};

	// Decode hex codes and print the 
	// instructions in spim as a full program
	printf("Program\n");
	decode_print (codes, filename);


	// Execute and print the return of the spim program
	printf("Output\n");
	decode_exec (codes, filename, registers);
	
	// Print the values left in all the
	// registers after execution
	printf("Registers After Execution\n");
	print_registers(registers);

	return 0;
}

/* returns int array array[0] is n_elements - 1 */
int *get_codes (FILE *file) {

	int *array = malloc(sizeof(int) * (MAX_CODES));

	int n_instructions = 1;
	while (n_instructions < MAX_CODES && fscanf(file, "%x", &array[n_instructions]) == 1) {
		n_instructions++;
	}

	// Check not overflowing array
	if (n_instructions == MAX_CODES) {
		fprintf(stderr, "Max instructions recieved.");
		exit (1);
	}
	array[0] = n_instructions;
	return array;
}

/* get the register number from the given code
using a mask and an offset */
int get_reg_from_code (int code, int offset, int mask) {
	return (mask & code) >> offset;
}

/* Set up decode process */
void decode_print (int codes[], char *filename) {

	// String in case of invalid instruction
	char *invalid = "%s:%d: invalid instruction code: %08x.\n";

	for (int PC = 1; PC < codes[0]; PC++) {
		if (do_decode_print(PC - 1, codes[PC]) == -1) {
			fprintf(stderr, invalid, filename, PC + 1, codes[PC]);
			exit (1);
		}
	}
}

/* decode instruction_code as a mips instruction
print the instruction to stdout */
int do_decode_print (int num, int instruction_code) {

	// Syscall
	if (instruction_code == 0x0000000c) {
		printf("%3d: syscall\n", num);
		return 0;
	}
	
	// calculate registers
	int s_reg = get_reg_from_code (instruction_code, S_OFFSET, S_MASK);
	int t_reg = get_reg_from_code (instruction_code, T_OFFSET, T_MASK);
	int d_reg = get_reg_from_code (instruction_code, D_OFFSET, D_MASK);

	// instruction num
	printf ("%3d: ", num);

	if ((instruction_code & TOP_MASK) == 0) {

		switch (instruction_code & BOTTOM_MASK) {
			
			case 0x20:
				printf("add  ");
				break;

			case 0x22:
				printf("sub  ");
				break;

			case 0x24:
				printf("and  ");
				break;

			case 0x25:
				printf("or   ");
				break;

			case 0x2a:
				printf("slt  ");
				break;
			
			default:
				return -1;
		}
		printf("$%d, $%d, $%d\n", d_reg, s_reg, t_reg);
		return 0;

	} else {
		// Getting I from lowest 16 bits
		int I = get_reg_from_code(instruction_code, 0, I_MASK);
		
		// Fixing I if negative (Signed int)
		if (I >= 32767) {
			I = -(65536 - I);
		}
		switch ((instruction_code & TOP_MASK) >> 26) {

			case 0x1c:
				printf("mul  ");
				printf("$%d, $%d, $%d\n", d_reg, s_reg, t_reg);
				return 0;

			case 0x4:
				printf("beq  ");
				printf("$%d, $%d, %d\n", s_reg, t_reg, I);
				return 0;

			case 0x5:
				printf("bne  ");
				printf("$%d, $%d, %d\n", s_reg, t_reg, I);
				return 0;

			case 0x8:
				printf("addi ");
				break;

			case 0xa:
				printf("slti ");
				break;
			case 0xc:
				printf("andi ");
				break;

			case 0xd:
				printf("ori  ");
				break;

			case 0xf:
				printf("lui  ");
				printf("$%d, %d\n", t_reg, I);
				return 0;

			default:
				return -1;
		}
		printf("$%d, $%d, %d\n", t_reg, s_reg, I);
		return 0;
	}
	
	// Invalid program
	return -1;
}

/* Set up decode process */
void decode_exec (int codes[], char *filename, int registers[]) {

	// String in case of invalid instruction
	char *invalid = "%s:%d: invalid instruction code: %08x.\n";

	for (int PC = 1; PC < codes[0] && PC > 0; PC++) {
		int exec_value = do_decode_exec (PC - 1, codes[PC], registers, &PC);
		if (exec_value == -1) {
			fprintf(stderr, invalid, filename, PC + 1, codes[PC]);
			exit (1);
		} else if (exec_value == -2) {
			registers[0] = 0;
			return;
		}
		registers[0] = 0;
	}
}

/* decode instruction_code as a mips instruction
execute the instructions */
int do_decode_exec (int num, int instruction_code, int registers[], int *PC) {

	// Syscall
	if (instruction_code == 0x0000000c) {
		int sys_value = sys_call(registers);
		if (sys_value == -1 || sys_value == 2) {
			return -2;
		}
		return 0;
	}
	
	// Calculate registers
	int s_reg = get_reg_from_code (instruction_code, S_OFFSET, S_MASK);
	int t_reg = get_reg_from_code (instruction_code, T_OFFSET, T_MASK);
	int d_reg = get_reg_from_code (instruction_code, D_OFFSET, D_MASK);

	// Top bits are 0
	if ((instruction_code & TOP_MASK) == 0) {
		
		switch (instruction_code & BOTTOM_MASK) {
			case 0x20:
				add (registers, d_reg, s_reg, t_reg);
				break;

			case 0x22:
				sub (registers, d_reg, s_reg, t_reg);
				break;

			case 0x24:
				and_op (registers, d_reg, s_reg, t_reg);
				break;

			case 0x25:
				or_op (registers, d_reg, s_reg, t_reg);
				break;

			case 0x2a:
				slt (registers, d_reg, s_reg, t_reg);
				break;
			
			default:
				return -1;
		}
		return 0;
		
	} else {
		// get I from lowest 16 bits
		int I = get_reg_from_code(instruction_code, 0, I_MASK);
		// fixing if negative
		if (I >= 32767) {
			I = -(65536 - I);
		}
		switch ((instruction_code & TOP_MASK) >> 26) {
			
			case 0x1c:
				mul (registers, d_reg, s_reg, t_reg);
				return 0;

			case 0x4:
				beq (registers, s_reg, t_reg, I - 1, PC);
				return 0;

			case 0x5:
				bne (registers, s_reg, t_reg, I - 1, PC);
				return 0;

			case 0x8:
				addi (registers, t_reg, s_reg, I);
				break;

			case 0xa:
				slti (registers, t_reg, s_reg, I);
				break;

			case 0xc:
				andi (registers, t_reg, s_reg, I);
				break;

			case 0xd:
				ori (registers, t_reg, s_reg, I);
				break;

			case 0xf:
				lui (registers, t_reg, I);
				return 0;

			default:
				return -1;
		}
		return 0;
	}
	return -1;
}

/* Prints all registers that do not contain
a 0 value */
void print_registers(int registers[]) {
	for (int i = 0; i < 32; i++) {
		if (registers[i] == 0) continue;
		printf ("$%-2d = %d\n", i, registers[i]);
	}
}