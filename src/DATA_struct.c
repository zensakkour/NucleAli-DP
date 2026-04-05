#include "DATA_struct.h"

static void compact_without_char(const char* src, char* dst, char c) {
    size_t out = 0;
    size_t i = 0;
    while (src[i] != '\0') {
        if (src[i] != c) {
            dst[out++] = src[i];
        }
        ++i;
    }
    dst[out] = '\0';
}

duo_chaine* creer_duo_chaine(char* x, char* y, int n, int m) {
    duo_chaine* res = (duo_chaine*)malloc(sizeof(duo_chaine));
    if (res == NULL) {
        fprintf(stderr, "Allocation error for duo_chaine\n");
        return NULL;
    }

    res->x = x;
    res->y = y;
    res->x[n] = '\0';
    res->y[m] = '\0';
    res->n = n;
    res->m = m;
    return res;
}

void supprimer_duo_chaine(duo_chaine* duo) {
    if (duo == NULL) {
        return;
    }
    free(duo->x);
    free(duo->y);
    free(duo);
}

void afficher_duo_chaine(duo_chaine* duo) {
    if (duo == NULL) {
        printf("duo is NULL\n");
        return;
    }
    printf("n=%d, m=%d\nx=%s\ny=%s\n", duo->n, duo->m, duo->x, duo->y);
}

Align* creer_alignement(int n, int m) {
    Align* res = (Align*)malloc(sizeof(Align));
    if (res == NULL) {
        fprintf(stderr, "Allocation error for Align\n");
        return NULL;
    }

    char* x = (char*)malloc((size_t)(n + m + 1) * sizeof(char));
    char* y = (char*)malloc((size_t)(n + m + 1) * sizeof(char));
    if (x == NULL || y == NULL) {
        free(x);
        free(y);
        free(res);
        fprintf(stderr, "Allocation error for alignment strings\n");
        return NULL;
    }

    for (int i = 0; i < n + m; ++i) {
        x[i] = '|';
        y[i] = '|';
    }

    x[n + m] = '\0';
    y[n + m] = '\0';
    res->x = x;
    res->y = y;
    res->iter = n + m;
    res->dist = 0;
    res->size = n + m;
    return res;
}

void afficher_alignement(Align* algn) {
    if (algn == NULL || algn->x == NULL || algn->y == NULL) {
        printf("alignment is NULL\n");
        return;
    }

    printf("%s\n%s\nEdit distance = %d\n", algn->x, algn->y, algn->dist);
}

void supprimer_alignement(Align* algn) {
    if (algn == NULL) {
        return;
    }
    free(algn->x);
    free(algn->y);
    free(algn);
}

void check_sanity(Align* res, duo_chaine* duo) {
    if (res == NULL || duo == NULL || res->x == NULL || res->y == NULL || duo->x == NULL || duo->y == NULL) {
        printf("sanity check failed: invalid input\n");
        return;
    }

    char* compact_x = (char*)malloc(strlen(res->x) + 1);
    char* compact_y = (char*)malloc(strlen(res->y) + 1);
    if (compact_x == NULL || compact_y == NULL) {
        free(compact_x);
        free(compact_y);
        printf("sanity check failed: allocation error\n");
        return;
    }

    compact_without_char(res->x, compact_x, '-');
    compact_without_char(res->y, compact_y, '-');

    if (strcmp(compact_x, duo->x) != 0 || strcmp(compact_y, duo->y) != 0) {
        printf("sanity check failed\n");
    } else {
        printf("sanity check: ok\n");
    }

    free(compact_x);
    free(compact_y);
}

void deletechar(char* s, char c) {
    if (s == NULL) {
        return;
    }

    size_t read_index = 0;
    size_t write_index = 0;
    while (s[read_index] != '\0') {
        if (s[read_index] != c) {
            s[write_index++] = s[read_index];
        }
        ++read_index;
    }
    s[write_index] = '\0';
}

