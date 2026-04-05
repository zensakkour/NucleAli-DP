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

duo_chaine* lire_genome(char* file_name);

int read_len_max(char* name_f);
void add_LF(txtFile** l, txtFile* f);
txtFile* read_all_Fnam(const char* Dir_name, int mx);
void delete_List_Files(txtFile* L);

long double get_memory_usage();

#endif


