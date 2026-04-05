#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "DATA_struct.h"

#define C_DEL 2
#define C_INS 2
#define C_SUB_CONCORDANT 3
#define C_SUB_NON_CONCORDANT 4
#define INF 1000000000

typedef struct AlignmentCosts {
    int insertion;
    int deletion;
    int substitution_concordant;
    int substitution_non_concordant;
} AlignmentCosts;

/* Cost configuration */
int alignment_set_costs(AlignmentCosts costs);
AlignmentCosts alignment_get_costs(void);

/* Legacy API */
int lettres_concordantes(char a, char b);
int cout_substitution(char a, char b);
int min3(int a, int b, int c);

int dist_naif_rec(char* x, char* y, int i, int j, int c, int dist);
int dist_naif(char* x, char* y);

int dist_1(char* x, char* y, int** DP);
Align* sol_1(char* x, char* y, int** DP);

int Dist_2(char* x, char* y, int n, int m, int** DP);
void pM(int** D, int n, int m);

Align* PROG_DYN(duo_chaine* duo);
char* mot_gaps(int k);
Align* align_lettre_mot(char x, char* y, int m);
int coupure(char* x, char* y, int n, int m, int** DP, int** I);
int test_coup(duo_chaine* duo);
Align* SOL_2(char* x, char* y, int n, int m, int** Dist, int** I);
Align* PROG_DYN_SOL2(duo_chaine* duo);

int calcul_cout(Align* res);

/* Readability aliases (same behavior, clearer naming) */
int alignment_are_concordant(char a, char b);
int alignment_substitution_cost(char a, char b);
int alignment_distance_naive(char* x, char* y);
int alignment_distance_full_dp(char* x, char* y, int** dp_matrix);
int alignment_distance_linear_dp(char* x, char* y, int n, int m, int** dp_rows);
Alignment* alignment_solve_full(SequencePair* pair);
Alignment* alignment_solve_linear(SequencePair* pair);
int alignment_compute_cost(Alignment* alignment);

#endif

