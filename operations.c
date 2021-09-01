#include <stdio.h>
#include <stdint.h>

#include "operations.h"

int sys_call (int reg[]) {
    if (reg[2] == 1) {
        // Print integer
        printf ("%d", reg[4]);
        return 0;
    } else if (reg[2] == 10) {
        // exit ()
        return 2;
    } else if (reg[2] == 11) {
        // print character
        int ch = 0xff & reg[4];
        printf ("%c", ch);
        return 0;
    } else {
        printf("Unknown system call: %d\n", reg[2]);
        return -1;
    }
    return -1;
}

void add (int reg[], int d_reg, int s_reg, int t_reg) {
    reg[d_reg] = reg[s_reg] + reg[t_reg];
}

void sub (int reg[], int d_reg, int s_reg, int t_reg) {
    reg[d_reg] = reg[s_reg] - reg[t_reg];
}

void and_op (int reg[], int d_reg, int s_reg, int t_reg) {
    reg[d_reg] = reg[s_reg] & reg[t_reg];
}

void or_op (int reg[], int d_reg, int s_reg, int t_reg) {
    reg[d_reg] = reg[s_reg] | reg[t_reg];
}

void slt (int reg[], int d_reg, int s_reg, int t_reg) {
    reg[d_reg] = 1 ? (reg[s_reg] < reg[t_reg]) : 0;
}

void mul (int reg[], int d_reg, int s_reg, int t_reg) {
    reg[d_reg] = reg[s_reg] * reg[t_reg];
}

void beq (int reg[], int s_reg, int t_reg, int I, int *PC) {
    if (reg[s_reg] == reg[t_reg]) *PC += I;
}

void bne (int reg[], int s_reg, int t_reg, int I, int *PC) {
    if (reg[s_reg] != reg[t_reg]) *PC += I;
}

void addi (int reg[], int t_reg, int s_reg, int I) {
    reg[t_reg] = reg[s_reg] + I;
}

void slti (int reg[], int t_reg, int s_reg, int I) {
    reg[t_reg] = (reg[s_reg] < I);
}

void andi (int reg[], int t_reg, int s_reg, int I) {
    reg[t_reg] = reg[s_reg] & I;
}

void ori (int reg[], int t_reg, int s_reg, int I) {
    reg[t_reg] = reg[s_reg] | I;
}

void lui (int reg[], int t_reg, int I) {
    reg[t_reg] = I << 16;
}