#ifndef PARSER_H
#define PARSER_H

#include "config.h"

void parse_config();
void load_full_build(struct project_t*, struct build_t*);

void argv_to_path(int, char**);
void parse_path(const char*);
void parse_query(const char*);

#endif