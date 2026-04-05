#ifndef INSTANCE_IO_H
#define INSTANCE_IO_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DATA_struct.h"

typedef struct _txtFile {
    char* Fname;
    int len;
    struct _txtFile* next;
} txtFile;
typedef txtFile FileEntry;

duo_chaine* lire_genome(char* file_name);

int read_len_max(char* name_f);
void add_LF(txtFile** l, txtFile* f);
txtFile* read_all_Fnam(const char* Dir_name, int mx);
void delete_List_Files(txtFile* L);

long double get_memory_usage();

/* Readability aliases (same behavior, clearer naming) */
SequencePair* read_genome_instance(char* file_name);
int read_instance_length_from_name(char* name_f);
void add_file_entry_sorted(FileEntry** list, FileEntry* entry);
FileEntry* read_instance_directory(const char* dir_name, int max_files);
void free_file_entries(FileEntry* list);

#endif


