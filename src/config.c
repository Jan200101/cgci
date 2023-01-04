#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "context.h"
#include "fs.h"
#include "config.h"

const char* build_string[] = {
    "Unknown",
    "In Progress",
    "Success",
    "Failure"
};

const char* build_class[] = {
    "unknown",
    "in-progress",
    "success",
    "failure"
};

struct config_t config;
struct project_t* current_project = NULL;
struct build_t* current_build = NULL;


void init_config()
{
    config.path_prefix = NULL;
    config.token = NULL;

    config.projects = NULL;
    config.project_count = 0;

    parse_config();

    if (context.project)
    {    
        for (size_t i = 0; i < config.project_count; ++i)
        {
            if (!strcmp(context.project, config.projects[i].name))
            {
                current_project = &config.projects[i];
                break;
            }
        }
    }

    if (context.index)
    {
        for (size_t i = 0; i < config.project_count; ++i)
        {
            for (size_t j = 0; j < config.projects[i].build_count; ++j)
            {
                if (!strcmp(context.index, config.projects[i].builds[j].name))
                {
                    current_build = &config.projects[i].builds[j];
                    load_full_build(current_project, current_build);
                    break;
                }
            }
        }
    }
}

void deinit_config()
{
    free(config.path_prefix);
    free(config.token);

    if (config.projects)
    {
        for (size_t i = 0; i < config.project_count; ++i)
        {
            free(config.projects[i].name);

            free(config.projects[i].script_path);

            free(config.projects[i].description);

            if (config.projects[i].builds)
            {
                for (size_t j = 0; j < config.projects[i].build_count; ++j)
                {
                    free(config.projects[i].builds[j].name);

                    free(config.projects[i].builds[j].log);
                }

                free(config.projects[i].builds);
            }
        }
        free(config.projects);
    }
}

char* cache_dir()
{
    char* dir = "cache/"NAME;
    if (isDir(dir))
        return strdup(dir);

    dir = CACHE_DIR;

    if (!isDir(CACHE_DIR) && makeDir(CACHE_DIR))
    {
        return NULL;
    }

    return strdup(dir);
}

char* project_dir(char* project)
{
    if (!project)
        return NULL;

    char* cache = cache_dir();

    if (!cache)
        return cache;

    size_t size = strlen(cache) + 1 + strlen(project) + 1;
    cache = realloc(cache, size);
    strncat(cache, "/", size - strlen(cache) - 1);
    strncat(cache, project, size - strlen(cache) - 1);
    cache[size] = '\0';

    return cache;
}

char* build_dir(char* project, char* build)
{
    if (!project || !build)
        return NULL;

    char* project_path = project_dir(project);

    if (!project_path)
        return project_path;

    size_t size = strlen(project_path) + 1 + strlen(build) ;
    project_path = realloc(project_path, size+1);
    strncat(project_path, "/", size);
    strncat(project_path, build, size);
    project_path[size] = '\0';

    return project_path;
}