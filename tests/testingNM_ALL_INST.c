#include "../src/alignment.h"
#include "../src/instance_io.h"

int main(void) {
    clock_t start;
    clock_t end;
    double elapsed_seconds = 0.0;

    txtFile* all_files = read_all_Fnam("../instances", 50);
    txtFile* current = all_files;

    FILE* time_file = fopen("../data/Temps_NM.txt", "w");
    FILE* mem_file = fopen("../data/mem_NM.txt", "w");
    if (time_file == NULL || mem_file == NULL) {
        delete_List_Files(all_files);
        if (time_file != NULL) {
            fclose(time_file);
        }
        if (mem_file != NULL) {
            fclose(mem_file);
        }
        return 1;
    }

    long double base_mem = get_memory_usage();

    while (current != NULL && elapsed_seconds < 60.0) {
        char path[256];
        int len = current->len;
        snprintf(path, sizeof(path), "../instances/%s", current->Fname);

        duo_chaine* duo = lire_genome(path);
        if (duo == NULL) {
            current = current->next;
            continue;
        }

        start = clock();
        int dist = dist_naif(duo->x, duo->y);
        end = clock();

        elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
        long double current_mem = get_memory_usage() - base_mem;

        printf("%s -> dist=%d time=%.4fs mem=%.2LfKB\n", path, dist, elapsed_seconds, current_mem);
        fprintf(time_file, "%d %.4f\n", len, elapsed_seconds);
        fprintf(mem_file, "%d %.4Lf\n", len, current_mem);

        supprimer_duo_chaine(duo);
        current = current->next;
    }

    fclose(time_file);
    fclose(mem_file);
    delete_List_Files(all_files);
    return 0;
}

