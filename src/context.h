#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdint.h>
#include <stdlib.h>

struct context_t {
    char* raw_path;

    char** path; /* NULLABLE */
    size_t path_length;

    // simplistic values
    // never allocated to
    char* project;
    char* action;
    char* index;
    char* extra;

    char* token;

    uint8_t debug;
};

extern struct context_t context;

void init_context();
void deinit_context();

#endif