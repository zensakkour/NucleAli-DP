#include "instance_io.h"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <dirent.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

static char* xstrdup(const char* s) {
    size_t len = strlen(s);
    char* out = (char*)malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    memcpy(out, s, len + 1);
    return out;
}

static int is_genome_char(int c) {
    return c == 'A' || c == 'C' || c == 'T' || c == 'G';
}

static int has_adn_extension(const char* name) {
    size_t len = strlen(name);
    if (len < 4) {
        return 0;
    }
    return strcmp(name + len - 4, ".adn") == 0;
}

static int read_sequence(FILE* f, char* out, int expected_len) {
    int count = 0;
    while (count < expected_len) {
        int c = fgetc(f);
        if (c == EOF) {
            return 0;
        }
        if (is_genome_char(c)) {
            out[count++] = (char)c;
        }
    }
    out[count] = '\0';
    return 1;
}

duo_chaine* lire_genome(char* file_name) {
    FILE* f = fopen(file_name, "r");
    if (f == NULL) {
        fprintf(stderr, "Read error for file: %s\n", file_name);
        return NULL;
    }

    int n = 0;
    int m = 0;

    if (fscanf(f, "%d", &n) != 1 || fscanf(f, "%d", &m) != 1 || n < 0 || m < 0) {
        fprintf(stderr, "Malformed header in file: %s\n", file_name);
        fclose(f);
        return NULL;
    }

    char* x = (char*)malloc((size_t)n + 1);
    char* y = (char*)malloc((size_t)m + 1);
    if (x == NULL || y == NULL) {
        free(x);
        free(y);
        fclose(f);
        fprintf(stderr, "Allocation error while reading: %s\n", file_name);
        return NULL;
    }

    if (!read_sequence(f, x, n) || !read_sequence(f, y, m)) {
        fprintf(stderr, "Malformed genome sequence in file: %s\n", file_name);
        free(x);
        free(y);
        fclose(f);
        return NULL;
    }

    duo_chaine* duo = creer_duo_chaine(x, y, n, m);
    fclose(f);
    return duo;
}

void add_LF(txtFile** l, txtFile* f) {
    txtFile* tmp = *l;

    if (*l == NULL) {
        *l = f;
        f->next = NULL;
        return;
    }

    if (tmp->len > f->len) {
        f->next = tmp;
        *l = f;
        return;
    }

    while (tmp->next != NULL && tmp->next->len < f->len) {
        tmp = tmp->next;
    }

    if (tmp->next == NULL) {
        tmp->next = f;
        f->next = NULL;
    } else {
        txtFile* next_file = tmp->next;
        tmp->next = f;
        f->next = next_file;
    }
}

int read_len_max(char* name_f) {
    unsigned int parsed_len = 0;
    if (sscanf(name_f, "Inst_%u_", &parsed_len) == 1) {
        return (int)parsed_len;
    }
    return 0;
}

txtFile* read_all_Fnam(const char* Dir_name, int mx) {
    txtFile* list_Fnames = NULL;
    int i = 0;
#ifdef _WIN32
    char pattern[MAX_PATH];
    if (snprintf(pattern, sizeof(pattern), "%s\\*", Dir_name) >= (int)sizeof(pattern)) {
        fprintf(stderr, "directory path too long: %s\n", Dir_name);
        return NULL;
    }

    WIN32_FIND_DATAA data;
    HANDLE hFind = FindFirstFileA(pattern, &data);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "error opening directory: %s\n", Dir_name);
        return NULL;
    }

    do {
        if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0) {
            continue;
        }
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            continue;
        }
        if (!has_adn_extension(data.cFileName)) {
            continue;
        }

        txtFile* f = (txtFile*)malloc(sizeof(txtFile));
        if (f == NULL) {
            continue;
        }

        f->Fname = xstrdup(data.cFileName);
        if (f->Fname == NULL) {
            free(f);
            continue;
        }

        f->len = read_len_max(f->Fname);
        f->next = NULL;
        add_LF(&list_Fnames, f);
        ++i;
    } while (i < mx && FindNextFileA(hFind, &data));

    FindClose(hFind);
#else
    DIR* rep = opendir(Dir_name);
    if (rep == NULL) {
        fprintf(stderr, "error opening directory: %s\n", Dir_name);
        return NULL;
    }

    struct dirent* file = NULL;
    while (i < mx && (file = readdir(rep)) != NULL) {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }
        if (!has_adn_extension(file->d_name)) {
            continue;
        }

        txtFile* f = (txtFile*)malloc(sizeof(txtFile));
        if (f == NULL) {
            continue;
        }

        f->Fname = xstrdup(file->d_name);
        if (f->Fname == NULL) {
            free(f);
            continue;
        }

        f->len = read_len_max(f->Fname);
        f->next = NULL;
        add_LF(&list_Fnames, f);
        ++i;
    }

    closedir(rep);
#endif
    return list_Fnames;
}

void delete_List_Files(txtFile* L) {
    while (L != NULL) {
        txtFile* tmp = L->next;
        free(L->Fname);
        free(L);
        L = tmp;
    }
}

long double get_memory_usage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return (long double)pmc.PeakWorkingSetSize / 1024.0L;
    }
    return 0.0L;
#else
    struct rusage r_usage;
    int ret = getrusage(RUSAGE_SELF, &r_usage);
    if (ret != 0) {
        fprintf(stderr, "Error in getrusage. errno=%d\n", errno);
        return 0.0L;
    }
    return (long double)r_usage.ru_maxrss;
#endif
}

