#include <stdio.h>
#include <stdint.h>

int sys_call (int reg[]);

void add (int reg[], int d_reg, int s_reg, int t_reg);

void sub (int reg[], int d_reg, int s_reg, int t_reg);

void and_op (int reg[], int d_reg, int s_reg, int t_reg);

void or_op (int reg[], int d_reg, int s_reg, int t_reg);

void slt (int reg[], int d_reg, int s_reg, int t_reg);

void mul (int reg[], int d_reg, int s_reg, int t_reg);

void beq (int reg[], int s_reg, int t_reg, int I, int *PC);

void bne (int reg[], int s_reg, int t_reg, int I, int *PC);

void addi (int reg[], int t_reg, int s_reg, int I);

void slti (int reg[], int t_reg, int s_reg, int I);

void andi (int reg[], int t_reg, int s_reg, int I);

void ori (int reg[], int t_reg, int s_reg, int I);

void lui (int reg[], int t_reg, int I);