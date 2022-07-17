#ifndef UI_H
#define UI_H

#include <time.h>
#include <stdint.h>

#define HTML_START "<!DOCTYPE html><html>"
#define HTML_END "</html>"

void print_html();
void print_head();

void print_body();
void print_title();
void print_build_nav();
void print_build_info();
void print_build_trigger();
void print_build_list();
void print_project_nav();
void print_project_list();

void print_asset(const char*);

void strdifftime(time_t, time_t, char*, size_t);

#endif
