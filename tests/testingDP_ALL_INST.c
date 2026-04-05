#include "../src/alignment.h"
#include "../src/instance_io.h"

int main(void) {
    clock_t start;
    clock_t end;
    double elapsed_seconds = 0.0;

    txtFile* all_files = read_all_Fnam("../instances", 50);
    txtFile* current = all_files;

    FILE* sol1_file = fopen("../data/Temps_DP_SOL1_ALL_INST.txt", "w");
    FILE* sol2_file = fopen("../data/Temps_DP_SOL2_ALL_INST.txt", "w");
    if (sol1_file == NULL || sol2_file == NULL) {
        delete_List_Files(all_files);
        if (sol1_file != NULL) {
            fclose(sol1_file);
        }
        if (sol2_file != NULL) {
            fclose(sol2_file);
        }
        return 1;
    }

    while (current != NULL && elapsed_seconds < 20.0) {
        char path[256];
        snprintf(path, sizeof(path), "../instances/%s", current->Fname);

        duo_chaine* duo = lire_genome(path);
        if (duo == NULL) {
            current = current->next;
            continue;
        }

        printf("\n%s\n", path);

        start = clock();
        Align* sol1 = PROG_DYN(duo);
        end = clock();
        elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
        fprintf(sol1_file, "%d %.4f\n", current->len, elapsed_seconds);
        check_sanity(sol1, duo);

        start = clock();
        Align* sol2 = PROG_DYN_SOL2(duo);
        end = clock();
        elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
        fprintf(sol2_file, "%d %.4f\n", current->len, elapsed_seconds);
        check_sanity(sol2, duo);

        supprimer_alignement(sol1);
        supprimer_alignement(sol2);
        supprimer_duo_chaine(duo);

        current = current->next;
    }

    fclose(sol1_file);
    fclose(sol2_file);
    delete_List_Files(all_files);
    return 0;
}

