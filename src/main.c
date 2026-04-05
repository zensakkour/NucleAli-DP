#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "alignment.h"
#include "instance_io.h"

typedef struct CliOptions {
    const char* input_path;
    const char* batch_dir;
    const char* solver;
    const char* csv_path;
    int show_alignment;
    int json_output;
    int verify;
    AlignmentCosts costs;
    int has_custom_costs;
} CliOptions;

typedef struct RunResult {
    int edit_distance;
    int alignment_length;
    double runtime_seconds;
} RunResult;

static void print_usage(const char* prog) {
    printf("Usage:\n");
    printf("  %s --input <instance.adn> [options]\n", prog);
    printf("  %s --batch-dir <instances_dir> [options]\n", prog);
    printf("\nOptions:\n");
    printf("  --solver <sol1|sol2>           Solver (default: sol2)\n");
    printf("  --show-alignment               Print aligned strings\n");
    printf("  --verify                       Verify alignment consistency\n");
    printf("  --json                         Emit JSON output\n");
    printf("  --csv <path>                   Write batch CSV report\n");
    printf("  --cost-ins <int>               Insertion cost\n");
    printf("  --cost-del <int>               Deletion cost\n");
    printf("  --cost-sub-concordant <int>    Concordant substitution cost\n");
    printf("  --cost-sub-nonconcordant <int> Non-concordant substitution cost\n");
}

static int parse_int_arg(const char* value, int* out) {
    char* end = NULL;
    errno = 0;
    long v = strtol(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0' || v < -2147483647L || v > 2147483647L) {
        return 0;
    }
    *out = (int)v;
    return 1;
}

static void print_json_string(const char* s) {
    putchar('"');
    for (size_t i = 0; s[i] != '\0'; ++i) {
        if (s[i] == '\\' || s[i] == '"') {
            putchar('\\');
            putchar(s[i]);
        } else if (s[i] == '\n') {
            fputs("\\n", stdout);
        } else if (s[i] == '\r') {
            fputs("\\r", stdout);
        } else if (s[i] == '\t') {
            fputs("\\t", stdout);
        } else {
            putchar(s[i]);
        }
    }
    putchar('"');
}

static int alignment_matches_input(const Align* res, const duo_chaine* duo) {
    size_t xi = 0;
    size_t yi = 0;

    for (size_t i = 0; res->x[i] != '\0' && res->y[i] != '\0'; ++i) {
        if (res->x[i] != '-') {
            if (duo->x[xi] == '\0' || res->x[i] != duo->x[xi]) {
                return 0;
            }
            ++xi;
        }
        if (res->y[i] != '-') {
            if (duo->y[yi] == '\0' || res->y[i] != duo->y[yi]) {
                return 0;
            }
            ++yi;
        }
    }

    return duo->x[xi] == '\0' && duo->y[yi] == '\0';
}

static int solve_one(const char* input_path, const char* solver, int verify, Align** out_align, duo_chaine** out_duo, RunResult* out_result) {
    duo_chaine* duo = lire_genome((char*)input_path);
    if (duo == NULL) {
        return 0;
    }

    clock_t start = clock();
    Align* align = NULL;

    if (strcmp(solver, "sol1") == 0) {
        align = PROG_DYN(duo);
    } else if (strcmp(solver, "sol2") == 0) {
        align = PROG_DYN_SOL2(duo);
    } else {
        fprintf(stderr, "Unknown solver: %s\n", solver);
        supprimer_duo_chaine(duo);
        return 0;
    }

    clock_t end = clock();

    if (align == NULL) {
        supprimer_duo_chaine(duo);
        return 0;
    }

    if (verify) {
        if (!alignment_matches_input(align, duo)) {
            fprintf(stderr, "Verification failed: alignment does not match input sequences (%s)\n", input_path);
            supprimer_alignement(align);
            supprimer_duo_chaine(duo);
            return 0;
        }
        if (calcul_cout(align) != align->dist) {
            fprintf(stderr, "Verification failed: alignment cost mismatch (%s)\n", input_path);
            supprimer_alignement(align);
            supprimer_duo_chaine(duo);
            return 0;
        }
    }

    out_result->edit_distance = align->dist;
    out_result->alignment_length = align->size;
    out_result->runtime_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
    *out_align = align;
    *out_duo = duo;
    return 1;
}

static void print_single_text(const char* input_path, const char* solver, const RunResult* result, const Align* align, int show_alignment) {
    printf("Input              : %s\n", input_path);
    printf("Solver             : %s\n", solver);
    printf("Edit distance      : %d\n", result->edit_distance);
    printf("Alignment length   : %d\n", result->alignment_length);
    printf("Runtime (seconds)  : %.6f\n", result->runtime_seconds);

    if (show_alignment) {
        printf("\nAlignment:\n%s\n%s\n", align->x, align->y);
    }
}

static void print_single_json(const char* input_path, const char* solver, const RunResult* result, const Align* align, int show_alignment) {
    printf("{");
    printf("\"input\":");
    print_json_string(input_path);
    printf(",\"solver\":");
    print_json_string(solver);
    printf(",\"edit_distance\":%d", result->edit_distance);
    printf(",\"alignment_length\":%d", result->alignment_length);
    printf(",\"runtime_seconds\":%.6f", result->runtime_seconds);
    if (show_alignment) {
        printf(",\"alignment_x\":");
        print_json_string(align->x);
        printf(",\"alignment_y\":");
        print_json_string(align->y);
    }
    printf("}\n");
}

int main(int argc, char** argv) {
    CliOptions opts;
    opts.input_path = NULL;
    opts.batch_dir = NULL;
    opts.solver = "sol2";
    opts.csv_path = NULL;
    opts.show_alignment = 0;
    opts.json_output = 0;
    opts.verify = 0;
    opts.costs.insertion = C_INS;
    opts.costs.deletion = C_DEL;
    opts.costs.substitution_concordant = C_SUB_CONCORDANT;
    opts.costs.substitution_non_concordant = C_SUB_NON_CONCORDANT;
    opts.has_custom_costs = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            opts.input_path = argv[++i];
        } else if (strcmp(argv[i], "--batch-dir") == 0 && i + 1 < argc) {
            opts.batch_dir = argv[++i];
        } else if (strcmp(argv[i], "--solver") == 0 && i + 1 < argc) {
            opts.solver = argv[++i];
        } else if (strcmp(argv[i], "--show-alignment") == 0) {
            opts.show_alignment = 1;
        } else if (strcmp(argv[i], "--verify") == 0) {
            opts.verify = 1;
        } else if (strcmp(argv[i], "--json") == 0) {
            opts.json_output = 1;
        } else if (strcmp(argv[i], "--csv") == 0 && i + 1 < argc) {
            opts.csv_path = argv[++i];
        } else if (strcmp(argv[i], "--cost-ins") == 0 && i + 1 < argc) {
            if (!parse_int_arg(argv[++i], &opts.costs.insertion)) {
                fprintf(stderr, "Invalid value for --cost-ins\n");
                return 1;
            }
            opts.has_custom_costs = 1;
        } else if (strcmp(argv[i], "--cost-del") == 0 && i + 1 < argc) {
            if (!parse_int_arg(argv[++i], &opts.costs.deletion)) {
                fprintf(stderr, "Invalid value for --cost-del\n");
                return 1;
            }
            opts.has_custom_costs = 1;
        } else if (strcmp(argv[i], "--cost-sub-concordant") == 0 && i + 1 < argc) {
            if (!parse_int_arg(argv[++i], &opts.costs.substitution_concordant)) {
                fprintf(stderr, "Invalid value for --cost-sub-concordant\n");
                return 1;
            }
            opts.has_custom_costs = 1;
        } else if (strcmp(argv[i], "--cost-sub-nonconcordant") == 0 && i + 1 < argc) {
            if (!parse_int_arg(argv[++i], &opts.costs.substitution_non_concordant)) {
                fprintf(stderr, "Invalid value for --cost-sub-nonconcordant\n");
                return 1;
            }
            opts.has_custom_costs = 1;
        } else {
            print_usage(argv[0]);
            return 1;
        }
    }

    if ((opts.input_path == NULL && opts.batch_dir == NULL) || (opts.input_path != NULL && opts.batch_dir != NULL)) {
        print_usage(argv[0]);
        return 1;
    }

    if (opts.has_custom_costs && !alignment_set_costs(opts.costs)) {
        fprintf(stderr, "Invalid custom costs: costs must be non-negative integers\n");
        return 1;
    }

    if (opts.input_path != NULL) {
        Align* align = NULL;
        duo_chaine* duo = NULL;
        RunResult result;

        if (!solve_one(opts.input_path, opts.solver, opts.verify, &align, &duo, &result)) {
            return 1;
        }

        if (opts.json_output) {
            print_single_json(opts.input_path, opts.solver, &result, align, opts.show_alignment);
        } else {
            print_single_text(opts.input_path, opts.solver, &result, align, opts.show_alignment);
        }

        supprimer_alignement(align);
        supprimer_duo_chaine(duo);
        return 0;
    }

    FILE* csv_file = NULL;
    if (opts.csv_path != NULL) {
        csv_file = fopen(opts.csv_path, "w");
        if (csv_file == NULL) {
            fprintf(stderr, "Cannot open CSV file: %s\n", opts.csv_path);
            return 1;
        }
        fprintf(csv_file, "instance,solver,edit_distance,alignment_length,runtime_seconds\n");
    }

    txtFile* list = read_all_Fnam(opts.batch_dir, 1000000);
    txtFile* current = list;

    int success_count = 0;
    int failed_count = 0;
    double total_runtime = 0.0;

    while (current != NULL) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", opts.batch_dir, current->Fname);

        Align* align = NULL;
        duo_chaine* duo = NULL;
        RunResult result;

        if (solve_one(path, opts.solver, opts.verify, &align, &duo, &result)) {
            ++success_count;
            total_runtime += result.runtime_seconds;

            if (csv_file != NULL) {
                fprintf(csv_file, "%s,%s,%d,%d,%.6f\n", current->Fname, opts.solver, result.edit_distance, result.alignment_length, result.runtime_seconds);
            }

            if (opts.json_output) {
                print_single_json(path, opts.solver, &result, align, 0);
            } else {
                print_single_text(path, opts.solver, &result, align, 0);
                printf("\n");
            }

            supprimer_alignement(align);
            supprimer_duo_chaine(duo);
        } else {
            ++failed_count;
            if (!opts.json_output) {
                fprintf(stderr, "Failed: %s\n", path);
            }
        }

        current = current->next;
    }

    if (csv_file != NULL) {
        fclose(csv_file);
    }

    if (opts.json_output) {
        printf("{");
        printf("\"summary\":{");
        printf("\"processed\":%d,", success_count + failed_count);
        printf("\"succeeded\":%d,", success_count);
        printf("\"failed\":%d,", failed_count);
        printf("\"total_runtime_seconds\":%.6f", total_runtime);
        printf("}}\n");
    } else {
        printf("Batch summary\n");
        printf("Processed         : %d\n", success_count + failed_count);
        printf("Succeeded         : %d\n", success_count);
        printf("Failed            : %d\n", failed_count);
        printf("Total runtime (s) : %.6f\n", total_runtime);
        if (success_count > 0) {
            printf("Avg runtime (s)   : %.6f\n", total_runtime / (double)success_count);
        }
    }

    delete_List_Files(list);
    return failed_count == 0 ? 0 : 1;
}
