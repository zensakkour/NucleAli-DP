#include "alignment.h"

#define COST_INS g_costs.insertion
#define COST_DEL g_costs.deletion
#define COST_SUB_CONCORDANT g_costs.substitution_concordant
#define COST_SUB_NON_CONCORDANT g_costs.substitution_non_concordant

static AlignmentCosts g_costs = {C_INS, C_DEL, C_SUB_CONCORDANT, C_SUB_NON_CONCORDANT};

static int min2(int a, int b) {
    return (a <= b) ? a : b;
}

static char* xstrdup(const char* s) {
    size_t len = strlen(s);
    char* out = (char*)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    memcpy(out, s, len + 1);
    return out;
}

int lettres_concordantes(char a, char b) {
    switch (a) {
        case 'A': return b == 'T';
        case 'C': return b == 'G';
        case 'T': return b == 'A';
        case 'G': return b == 'C';
        default: return 0;
    }
}

int alignment_set_costs(AlignmentCosts costs) {
    if (costs.insertion < 0 || costs.deletion < 0 || costs.substitution_concordant < 0 || costs.substitution_non_concordant < 0) {
        return 0;
    }
    g_costs = costs;
    return 1;
}

AlignmentCosts alignment_get_costs(void) {
    return g_costs;
}

int cout_substitution(char a, char b) {
    if (a == b) {
        return 0;
    }
    if (lettres_concordantes(a, b)) {
        return COST_SUB_CONCORDANT;
    }
    return COST_SUB_NON_CONCORDANT;
}

int min3(int a, int b, int c) {
    return min2(min2(a, b), c);
}

int dist_naif_rec(char* x, char* y, int i, int j, int c, int dist) {
    int lenX = (int)strlen(x);
    int lenY = (int)strlen(y);

    if (i == lenX && j == lenY) {
        if (c < dist || dist == INF) {
            dist = c;
        }
    } else {
        if (i < lenX && j < lenY) {
            dist = dist_naif_rec(x, y, i + 1, j + 1, c + cout_substitution(x[i], y[j]), dist);
        }
        if (i < lenX) {
            dist = dist_naif_rec(x, y, i + 1, j, c + COST_DEL, dist);
        }
        if (j < lenY) {
            dist = dist_naif_rec(x, y, i, j + 1, c + COST_INS, dist);
        }
    }
    return dist;
}

int dist_naif(char* x, char* y) {
    return dist_naif_rec(x, y, 0, 0, 0, INF);
}

int dist_1(char* x, char* y, int** DP) {
    int n = (int)strlen(x);
    int m = (int)strlen(y);

    DP[0][0] = 0;
    for (int i = 1; i <= n; ++i) {
        DP[i][0] = DP[i - 1][0] + COST_DEL;
    }
    for (int j = 1; j <= m; ++j) {
        DP[0][j] = DP[0][j - 1] + COST_INS;
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            int INS = DP[i][j - 1] + COST_INS;
            int DEL = DP[i - 1][j] + COST_DEL;
            int SUB = DP[i - 1][j - 1] + cout_substitution(x[i - 1], y[j - 1]);
            DP[i][j] = min3(INS, DEL, SUB);
        }
    }

    return DP[n][m];
}

Align* sol_1(char* x, char* y, int** D) {
    int n = (int)strlen(x);
    int m = (int)strlen(y);
    int i = n;
    int j = m;

    Align* res = creer_alignement(n, m);
    if (res == NULL) {
        return NULL;
    }

    res->iter = n + m - 1;
    res->dist = D[n][m];

    while (i > 0 && j > 0) {
        if (D[i][j] == D[i][j - 1] + COST_INS) {
            res->x[res->iter] = '-';
            res->y[res->iter] = y[j - 1];
            --res->iter;
            --j;
        } else if (D[i][j] == D[i - 1][j] + COST_DEL) {
            res->x[res->iter] = x[i - 1];
            res->y[res->iter] = '-';
            --res->iter;
            --i;
        } else if (D[i][j] == D[i - 1][j - 1] + cout_substitution(x[i - 1], y[j - 1])) {
            res->x[res->iter] = x[i - 1];
            res->y[res->iter] = y[j - 1];
            --res->iter;
            --i;
            --j;
        } else {
            supprimer_alignement(res);
            return NULL;
        }
    }

    while (i > 0) {
        res->x[res->iter] = x[i - 1];
        res->y[res->iter] = '-';
        --res->iter;
        --i;
    }

    while (j > 0) {
        res->x[res->iter] = '-';
        res->y[res->iter] = y[j - 1];
        --res->iter;
        --j;
    }

    deletechar(res->x, '|');
    deletechar(res->y, '|');
    return res;
}

void pM(int** D, int n, int m) {
    printf("Distance matrix\n");
    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= m; ++j) {
            printf("%d ", D[i][j]);
        }
        printf("\n");
    }
}

Align* PROG_DYN(duo_chaine* duo) {
    if (duo == NULL) {
        printf("input duo is NULL\n");
        return NULL;
    }

    size_t n = strlen(duo->x);
    size_t m = strlen(duo->y);

    int** Dist = (int**)malloc((n + 1) * sizeof(int*));
    if (Dist == NULL) {
        return NULL;
    }

    for (size_t i = 0; i <= n; ++i) {
        Dist[i] = (int*)malloc((m + 1) * sizeof(int));
        if (Dist[i] == NULL) {
            while (i > 0) {
                --i;
                free(Dist[i]);
            }
            free(Dist);
            return NULL;
        }
    }

    dist_1(duo->x, duo->y, Dist);
    Align* res = sol_1(duo->x, duo->y, Dist);

    if (res == NULL) {
        for (size_t i = 0; i <= n; ++i) {
            free(Dist[i]);
        }
        free(Dist);
        return NULL;
    }

    int cost = calcul_cout(res);
    assert(cost == Dist[n][m]);

    for (size_t i = 0; i <= n; ++i) {
        free(Dist[i]);
    }
    free(Dist);

    return res;
}

void swap(int* i1, int* i2) {
    int tmp = *i1;
    *i1 = *i2;
    *i2 = tmp;
}

int Dist_2(char* x, char* y, int n, int m, int** DP) {
    int INS, DEL, SUB;
    int iter1 = 1;
    int iter0 = 0;

    DP[0][0] = 0;
    for (int j = 1; j <= m; ++j) {
        DP[0][j] = j * COST_INS;
    }

    for (int i = 1; i <= n; ++i) {
        DP[iter1][0] = DP[iter0][0] + COST_DEL;
        for (int j = 1; j <= m; ++j) {
            INS = DP[iter1][j - 1] + COST_INS;
            DEL = DP[iter0][j] + COST_DEL;
            SUB = DP[iter0][j - 1] + cout_substitution(x[i - 1], y[j - 1]);
            DP[iter1][j] = min3(INS, DEL, SUB);
        }
        swap(&iter0, &iter1);
    }

    return DP[iter0][m];
}

char* mot_gaps(int k) {
    char* res = (char*)malloc((k + 1) * sizeof(char));
    if (res == NULL) {
        return NULL;
    }

    for (int i = 0; i < k; ++i) {
        res[i] = '-';
    }
    res[k] = '\0';
    return res;
}

Align* align_lettre_mot(char x, char* y, int m) {
    int best_index = 0;
    int best_cost = INF;

    Align* res = (Align*)malloc(sizeof(Align));
    if (res == NULL) {
        return NULL;
    }

    for (int t = 0; t < m; ++t) {
        int current_cost = cout_substitution(x, y[t]);
        if (current_cost < best_cost) {
            best_index = t;
            best_cost = current_cost;
        }
        if (current_cost == 0) {
            break;
        }
    }

    if (best_cost < COST_DEL + COST_INS) {
        res->x = mot_gaps(m);
        res->y = xstrdup(y);
        if (res->x == NULL || res->y == NULL) {
            free(res->x);
            free(res->y);
            free(res);
            return NULL;
        }
        res->x[best_index] = x;
        res->size = m;
        return res;
    }

    res->x = mot_gaps(m + 1);
    res->y = (char*)malloc((size_t)(m + 2) * sizeof(char));
    if (res->x == NULL || res->y == NULL) {
        free(res->x);
        free(res->y);
        free(res);
        return NULL;
    }

    res->x[0] = x;
    res->y[0] = '-';
    for (int a = 0; a < m; ++a) {
        res->y[a + 1] = y[a];
    }
    res->y[m + 1] = '\0';
    res->size = m + 1;
    return res;
}

Align* concatener_alignements(Align* algn_1, Align* algn_2) {
    algn_1->size = algn_1->size + algn_2->size;
    algn_1->x = (char*)realloc(algn_1->x, (size_t)algn_1->size + 1);
    algn_1->y = (char*)realloc(algn_1->y, (size_t)algn_1->size + 1);

    if (algn_1->x == NULL || algn_1->y == NULL) {
        supprimer_alignement(algn_2);
        return algn_1;
    }

    algn_1->x[algn_1->size] = '\0';
    algn_1->y[algn_1->size] = '\0';
    strcat(algn_1->x, algn_2->x);
    strcat(algn_1->y, algn_2->y);

    supprimer_alignement(algn_2);
    return algn_1;
}

int coupure(char* x, char* y, int n, int m, int** DP, int** I) {
    int INS, DEL, SUB;
    int iE = n / 2;
    int iter0 = 0;
    int iter1 = 1;

    DP[0][0] = 0;
    I[0][0] = 0;
    for (int j = 1; j <= m; ++j) {
        I[0][j] = j;
        DP[0][j] = DP[0][j - 1] + COST_INS;
    }

    for (int i = 1; i <= n; ++i) {
        DP[iter1][0] = DP[iter0][0] + COST_DEL;
        I[iter1][0] = 0;
        if (i == iE + 1) {
            for (int j = 0; j <= m; ++j) {
                I[iter0][j] = j;
            }
        }

        for (int j = 1; j <= m; ++j) {
            INS = DP[iter1][j - 1] + COST_INS;
            DEL = DP[iter0][j] + COST_DEL;
            SUB = DP[iter0][j - 1] + cout_substitution(x[i - 1], y[j - 1]);
            DP[iter1][j] = min3(INS, DEL, SUB);

            if (DP[iter1][j] == SUB) {
                I[iter1][j] = I[iter0][j - 1];
            }
            if (DP[iter1][j] == DEL) {
                I[iter1][j] = I[iter0][j];
            }
            if (DP[iter1][j] == INS) {
                I[iter1][j] = I[iter1][j - 1];
            }
        }

        swap(&iter0, &iter1);
    }

    return I[iter0][m];
}

int test_coup(duo_chaine* duo) {
    if (duo == NULL) {
        printf("input duo is NULL\n");
        return 0;
    }

    size_t n = strlen(duo->x);
    size_t m = strlen(duo->y);

    int** Dist = (int**)malloc((n + 1) * sizeof(int*));
    int** I = (int**)malloc((n + 1) * sizeof(int*));

    if (Dist == NULL || I == NULL) {
        free(Dist);
        free(I);
        return 0;
    }

    for (size_t i = 0; i <= n; ++i) {
        Dist[i] = (int*)malloc((m + 1) * sizeof(int));
        I[i] = (int*)malloc((m + 1) * sizeof(int));
    }

    dist_1(duo->x, duo->y, Dist);
    Align* res = sol_1(duo->x, duo->y, Dist);

    assert(calcul_cout(res) == res->dist);

    int dist2 = Dist_2(duo->x, duo->y, (int)strlen(duo->x), (int)strlen(duo->y), Dist);
    printf("dist1=%d, dist2=%d\n", res->dist, dist2);
    afficher_alignement(res);

    int c = coupure(duo->x, duo->y, (int)n, (int)m, Dist, I);

    for (size_t i = 0; i <= n; ++i) {
        free(Dist[i]);
        free(I[i]);
    }

    free(Dist);
    free(I);
    supprimer_alignement(res);

    return c;
}

Align* SOL_2(char* x, char* y, int n, int m, int** Dist, int** I) {
    if (n == 0) {
        Align* algn_res = (Align*)malloc(sizeof(Align));
        if (algn_res == NULL) {
            return NULL;
        }
        algn_res->x = mot_gaps(m);
        algn_res->y = xstrdup(y);
        algn_res->size = m;
        return algn_res;
    }

    if (m == 0) {
        Align* algn_res = (Align*)malloc(sizeof(Align));
        if (algn_res == NULL) {
            return NULL;
        }
        algn_res->x = xstrdup(x);
        algn_res->y = mot_gaps(n);
        algn_res->size = n;
        return algn_res;
    }

    if (n == 1) {
        Align* res = align_lettre_mot(x[0], y, m);
        assert(res != NULL);
        return res;
    }

    int j = coupure(x, y, n, m, Dist, I);
    int i = n / 2;

    char* xx = (char*)malloc((size_t)i + 1);
    char* yy = (char*)malloc((size_t)j + 1);
    if (xx == NULL || yy == NULL) {
        free(xx);
        free(yy);
        return NULL;
    }

    strncpy(xx, x, (size_t)i);
    strncpy(yy, y, (size_t)j);
    xx[i] = '\0';
    yy[j] = '\0';

    Align* algn_1 = SOL_2(xx, yy, i, j, Dist, I);
    free(xx);
    free(yy);

    Align* algn_2 = SOL_2(x + i, y + j, n - i, m - j, Dist, I);
    return concatener_alignements(algn_1, algn_2);
}

Align* PROG_DYN_SOL2(duo_chaine* duo) {
    if (duo == NULL) {
        printf("input duo is NULL\n");
        return NULL;
    }

    int n = (int)strlen(duo->x);
    int m = (int)strlen(duo->y);

    int** Dist = (int**)malloc(2 * sizeof(int*));
    Dist[0] = (int*)malloc((m + 1) * sizeof(int));
    Dist[1] = (int*)malloc((m + 1) * sizeof(int));

    int** I = (int**)malloc(2 * sizeof(int*));
    I[0] = (int*)malloc((m + 1) * sizeof(int));
    I[1] = (int*)malloc((m + 1) * sizeof(int));

    if (Dist == NULL || Dist[0] == NULL || Dist[1] == NULL || I == NULL || I[0] == NULL || I[1] == NULL) {
        free(Dist[0]);
        free(Dist[1]);
        free(Dist);
        free(I[0]);
        free(I[1]);
        free(I);
        return NULL;
    }

    for (int t = 0; t <= m; ++t) {
        Dist[0][t] = 0;
        Dist[1][t] = 0;
        I[0][t] = 0;
        I[1][t] = 0;
    }

    int dist2 = Dist_2(duo->x, duo->y, n, m, Dist);
    Align* res = SOL_2(duo->x, duo->y, n, m, Dist, I);

    if (res == NULL) {
        free(Dist[0]);
        free(Dist[1]);
        free(Dist);
        free(I[0]);
        free(I[1]);
        free(I);
        return NULL;
    }

    res->dist = dist2;
    int cost = calcul_cout(res);
    assert(cost == dist2);

    free(Dist[0]);
    free(Dist[1]);
    free(Dist);

    free(I[0]);
    free(I[1]);
    free(I);

    return res;
}

int calcul_cout(Align* res) {
    int cout = 0;
    for (int i = 0; i < (int)strlen(res->x); ++i) {
        if (res->x[i] == res->y[i]) {
            continue;
        }
        if (res->x[i] == '-') {
            cout += COST_INS;
            continue;
        }
        if (res->y[i] == '-') {
            cout += COST_DEL;
            continue;
        }
        cout += cout_substitution(res->x[i], res->y[i]);
    }
    return cout;
}


