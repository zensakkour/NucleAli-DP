#include "../src/alignment.h"
#include "../src/instance_io.h"

int main(void) {
    clock_t start;
    clock_t end;
    double elapsed_seconds = 0.0;

    int** dist_matrix;
    int choice;
    int dist;

    char instance_name[256];
    char path_prefix[256] = "instances/";

    printf("1: Validate file parser\n");
    printf("2: Validate naive distance\n");
    printf("3: Runtime for naive distance\n");
    printf("4: Distance with Dist_1\n");
    printf("5: Distance with Dist_2\n");
    printf("6: Build alignment with SOL_1\n");
    printf("7: Build alignment with SOL_2\n");
    printf("8: Bonus checks\n");

    if (scanf("%d", &choice) != 1) {
        return 1;
    }

    duo_chaine* duo;
    Align* res;

    switch (choice) {
        case 1: {
            duo_chaine* test = lire_genome("instances/Inst_0000010_7.adn");
            afficher_duo_chaine(test);
            supprimer_duo_chaine(test);
            break;
        }

        case 2: {
            duo_chaine* inst_10_8 = lire_genome("instances/Inst_0000010_8.adn");
            dist = dist_naif(inst_10_8->x, inst_10_8->y);
            printf("Distance for 10_8: %d (expected: 2)\n", dist);
            supprimer_duo_chaine(inst_10_8);

            duo_chaine* inst_10_7 = lire_genome("instances/Inst_0000010_7.adn");
            dist = dist_naif(inst_10_7->x, inst_10_7->y);
            printf("Distance for 10_7: %d (expected: 8)\n", dist);
            supprimer_duo_chaine(inst_10_7);

            duo_chaine* inst_10_44 = lire_genome("instances/Inst_0000010_44.adn");
            dist = dist_naif(inst_10_44->x, inst_10_44->y);
            printf("Distance for 10_44: %d (expected: 10)\n", dist);
            supprimer_duo_chaine(inst_10_44);
            break;
        }

        case 3: {
            printf("Enter an instance name (example: Inst_0000013_89.adn):\n");
            scanf("%255s", instance_name);
            strcat(path_prefix, instance_name);

            duo = lire_genome(path_prefix);
            start = clock();
            dist = dist_naif(duo->x, duo->y);
            end = clock();

            elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
            printf("Runtime: %.2fs | Edit distance: %d\n", elapsed_seconds, dist);
            supprimer_duo_chaine(duo);
            break;
        }

        case 4: {
            printf("Enter an instance name (example: Inst_0000013_89.adn):\n");
            scanf("%255s", instance_name);
            strcat(path_prefix, instance_name);

            duo = lire_genome(path_prefix);
            dist_matrix = (int**)malloc((size_t)(duo->n + 1) * sizeof(int*));
            for (int i = 0; i <= duo->n; ++i) {
                dist_matrix[i] = (int*)malloc((size_t)(duo->m + 1) * sizeof(int));
            }

            printf("Distance (Dist_1) = %d\n", dist_1(duo->x, duo->y, dist_matrix));

            for (int i = 0; i <= duo->n; ++i) {
                free(dist_matrix[i]);
            }
            free(dist_matrix);
            supprimer_duo_chaine(duo);
            break;
        }

        case 5: {
            printf("Enter an instance name (example: Inst_0000013_89.adn):\n");
            scanf("%255s", instance_name);
            strcat(path_prefix, instance_name);

            duo = lire_genome(path_prefix);
            dist_matrix = (int**)malloc(2 * sizeof(int*));
            dist_matrix[0] = (int*)malloc((size_t)(duo->m + 1) * sizeof(int));
            dist_matrix[1] = (int*)malloc((size_t)(duo->m + 1) * sizeof(int));

            printf("Distance (Dist_2) = %d\n", Dist_2(duo->x, duo->y, duo->n, duo->m, dist_matrix));

            free(dist_matrix[0]);
            free(dist_matrix[1]);
            free(dist_matrix);
            supprimer_duo_chaine(duo);
            break;
        }

        case 6: {
            printf("Enter an instance name (example: Inst_0000013_89.adn):\n");
            scanf("%255s", instance_name);
            strcat(path_prefix, instance_name);

            duo = lire_genome(path_prefix);
            start = clock();
            res = PROG_DYN(duo);
            end = clock();

            elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
            afficher_alignement(res);
            check_sanity(res, duo);
            printf("Runtime PROG_DYN: %.2fs\n", elapsed_seconds);

            supprimer_duo_chaine(duo);
            supprimer_alignement(res);
            break;
        }

        case 7: {
            printf("Enter an instance name (example: Inst_0000013_89.adn):\n");
            scanf("%255s", instance_name);
            strcat(path_prefix, instance_name);

            duo = lire_genome(path_prefix);
            start = clock();
            res = PROG_DYN_SOL2(duo);
            end = clock();

            elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
            afficher_alignement(res);
            check_sanity(res, duo);
            printf("Runtime PROG_DYN_SOL2: %.2fs\n", elapsed_seconds);

            supprimer_alignement(res);
            supprimer_duo_chaine(duo);
            break;
        }

        case 8: {
            duo = lire_genome("instances/Inst_0000125_BO.adn");
            res = PROG_DYN_SOL2(duo);
            int alignment_cost = calcul_cout(res);
            int bonus = (duo->n - duo->m) * C_DEL;
            printf("Inst_0000125_BO -> alignment=%d expected=%d\n", alignment_cost, bonus);
            assert(bonus == alignment_cost);
            supprimer_duo_chaine(duo);
            supprimer_alignement(res);

            duo = lire_genome("instances/Inst_0000200_BO.adn");
            res = PROG_DYN_SOL2(duo);
            alignment_cost = calcul_cout(res);
            bonus = (duo->n - duo->m) * C_DEL;
            printf("Inst_0000200_BO -> alignment=%d expected=%d\n", alignment_cost, bonus);
            assert(bonus == alignment_cost);
            supprimer_duo_chaine(duo);
            supprimer_alignement(res);

            duo = lire_genome("instances/Inst_0000250_BO.adn");
            res = PROG_DYN_SOL2(duo);
            alignment_cost = calcul_cout(res);
            bonus = (duo->n - duo->m) * C_DEL;
            printf("Inst_0000250_BO -> alignment=%d expected=%d\n", alignment_cost, bonus);
            assert(bonus == alignment_cost);
            supprimer_duo_chaine(duo);
            supprimer_alignement(res);
            break;
        }

        default:
            printf("Unknown choice\n");
            break;
    }

    return 0;
}

