#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>
#include <time.h>

enum build_status {
    STATUS_UNKNOWN,
    STATUS_INPROGRESS,
    STATUS_SUCCESS,
    STATUS_FAILURE
};
extern const char* build_string[];
extern const char* build_class[];

struct build_t {
    char* name;
    time_t timestamp;
    time_t completion;
    enum build_status status;
    char* log;
};

struct project_t {
    char* name;
    char* script_path;
    char* description;
    char* token;

    struct build_t* builds;
    size_t build_count;
};

struct config_t {
    char* path_prefix;
    char* token;

    struct project_t* projects;
    size_t project_count;
};

extern struct config_t config;
extern struct project_t* current_project;
extern struct build_t* current_build;

void init_config();
void deinit_config();

char* cache_dir();
char* project_dir(char* project);
char* build_dir(char* project, char* build);
    
#endif