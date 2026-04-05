#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _duo_chaine {
    char* x;
    char* y;
    int n;
    int m;
} duo_chaine;

duo_chaine* creer_duo_chaine(char* x, char* y, int n, int m);
void supprimer_duo_chaine(duo_chaine* duo);
void afficher_duo_chaine(duo_chaine* duo);

typedef struct _Align {
    char* x;
    char* y;
    int size;
    int dist;
    int iter;
} Align;

Align* creer_alignement(int n, int m);
void supprimer_alignement(Align* algn);
void afficher_alignement(Align* algn);

void check_sanity(Align* res, duo_chaine* duo);
void deletechar(char* s, char c);

#endif

