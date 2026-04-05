#include "../src/alignment.h"
#include "../src/instance_io.h"

int main(void) {
    clock_t start;
    clock_t end;
    double elapsed_seconds = 0.0;

    int compared = 0;
    txtFile* all_files = read_all_Fnam("../instances", 50);
    txtFile* current = all_files;

    FILE* dist1_file = fopen("../data/Temps_DIST1_ALL_INST.txt", "w");
    FILE* dist2_file = fopen("../data/Temps_DIST2_ALL_INST.txt", "w");
    if (dist1_file == NULL || dist2_file == NULL) {
        delete_List_Files(all_files);
        if (dist1_file != NULL) {
            fclose(dist1_file);
        }
        if (dist2_file != NULL) {
            fclose(dist2_file);
        }
        return 1;
    }

    while (current != NULL && elapsed_seconds < 200.0 && compared < 45) {
        ++compared;

        char path[256];
        snprintf(path, sizeof(path), "../instances/%s", current->Fname);
        duo_chaine* duo = lire_genome(path);
        if (duo == NULL) {
            current = current->next;
            continue;
        }

        int** dist = (int**)malloc((size_t)(duo->n + 1) * sizeof(int*));
        for (int i = 0; i <= duo->n; ++i) {
            dist[i] = (int*)malloc((size_t)(duo->m + 1) * sizeof(int));
        }

        start = clock();
        dist_1(duo->x, duo->y, dist);
        end = clock();
        elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
        fprintf(dist1_file, "%d %.4f\n", current->len, elapsed_seconds);

        for (int i = 0; i <= duo->n; ++i) {
            free(dist[i]);
        }
        free(dist);

        dist = (int**)malloc(2 * sizeof(int*));
        dist[0] = (int*)malloc((size_t)(duo->m + 1) * sizeof(int));
        dist[1] = (int*)malloc((size_t)(duo->m + 1) * sizeof(int));

        start = clock();
        Dist_2(duo->x, duo->y, duo->n, duo->m, dist);
        end = clock();
        elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
        fprintf(dist2_file, "%d %.4f\n", current->len, elapsed_seconds);

        free(dist[0]);
        free(dist[1]);
        free(dist);

        supprimer_duo_chaine(duo);
        current = current->next;
    }

    fclose(dist1_file);
    fclose(dist2_file);
    delete_List_Files(all_files);
    return 0;
}

