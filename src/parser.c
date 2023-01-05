#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>

#include "config.h"
#include "context.h"
#include "fs.h"
#include "parser.h"

void argv_to_path(int argc, char** argv)
{
    char* arg_path = malloc(1);
    size_t arg_path_length = 1;
    *arg_path = '\0';

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] != '\0')
        {
            arg_path_length += strlen(argv[i]) + 1;

            arg_path = realloc(arg_path, sizeof(char) * arg_path_length);

            strncat(arg_path, "/", arg_path_length - strlen(arg_path));
            strncat(arg_path, argv[i], arg_path_length - strlen(arg_path));
        }
    }

    if (arg_path_length)
    {
        setenv("PATH_INFO", arg_path, 1);
    }
    free(arg_path);
}

void parse_config()
{
    FILE* fd = fopen("/etc/cgcirc", "r");
    if (!fd)
    {
        fd = fopen("cgcirc", "r");
        if (!fd)
            return;
    }

    fseek(fd, 0L, SEEK_END);
    size_t file_size = (size_t)ftell(fd);
    rewind(fd);

    char* buf = malloc(file_size+1);
    assert(buf);

    fread(buf, sizeof(char*), file_size, fd);
    buf[file_size] = '\0';

    fclose(fd);

    char* head = buf;
    char* tail = head;

    char* key = NULL;
    char* value = NULL;

    while(*tail)
    {
        assert((size_t)(tail - buf) <= file_size);

        if (*head == '#')
        {
            while (*tail != '\0' && *tail != '\n')
            {
                assert((size_t)(tail - buf) <= file_size);
                ++tail;
            }

            head = ++tail;
            continue;
        }
        else if (*tail == '=')
        {
            key = head;
            *tail = '\0';
            head = tail+1;;
        }
        else if (*tail == '\0' || *tail == '\n')
        {
            if (key)
            {
                value = head;
            }
            *tail = '\0';
            head = tail+1;;
        }

        ++tail;

        if (key && value)
        {
            if (!strcmp(key, "token"))
            {
                free(config.token);
                config.token = strdup(value);
            }
            else if (!strcmp(key, "path-prefix"))
            {
                free(config.path_prefix);
                config.path_prefix = strdup(value);
            }
            else if (!strcmp(key, "project.name"))
            {
                ++config.project_count;

                config.projects = realloc(config.projects, config.project_count * sizeof(struct project_t));
                assert(config.projects);

                config.projects[config.project_count-1].name = strdup(value);
                config.projects[config.project_count-1].script_path = NULL;
                config.projects[config.project_count-1].description = NULL;
                config.projects[config.project_count-1].token = NULL;
            }
            else if (!strcmp(key, "project.script"))
            {
                if (config.project_count > 0)
                {
                    free(config.projects[config.project_count-1].script_path);
                    config.projects[config.project_count-1].script_path = strdup(value);
                }
            }
            else if (!strcmp(key, "project.description"))
            {
                if (config.project_count > 0)
                {
                    free(config.projects[config.project_count-1].description);
                    config.projects[config.project_count-1].description = strdup(value);   
                }
            }
            else if (!strcmp(key, "project.token"))
            {
                if (config.project_count > 0)
                {
                    free(config.projects[config.project_count-1].token);
                    config.projects[config.project_count-1].token = strdup(value);   
                }
            }

            key = NULL;
            value = NULL;
        }
    }

    char* project = NULL;
    char* build = NULL;
    for (size_t i = 0; i < config.project_count; ++i)
    {
        config.projects[i].build_count = 0;
        config.projects[i].builds = NULL;

        project = project_dir(config.projects[i].name);

        if (!isDir(project))
        {
            free(project);
            continue;
        }

        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir(project)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_name[0] == '.') continue;

                build = build_dir(config.projects[i].name, ent->d_name);
                size_t build_size = strlen(build);
                build = realloc(build, (build_size + 11 + 1) * sizeof(char));

                config.projects[i].build_count++;
                config.projects[i].builds = realloc(config.projects[i].builds, config.projects[i].build_count * sizeof(struct build_t));

                config.projects[i].builds[config.projects[i].build_count-1].name = strdup(ent->d_name);
                config.projects[i].builds[config.projects[i].build_count-1].timestamp = 0;
                config.projects[i].builds[config.projects[i].build_count-1].completion = 0;
                config.projects[i].builds[config.projects[i].build_count-1].status = STATUS_UNKNOWN;
                config.projects[i].builds[config.projects[i].build_count-1].log = NULL;

                strcat(build, "/timestamp");
                fd = fopen(build, "rb");
                if (fd)
                {
                    fread(&config.projects[i].builds[config.projects[i].build_count-1].timestamp, sizeof(time_t), 1, fd);
                    fclose(fd);
                }
                build[build_size] = '\0';

                strcat(build, "/completion");
                fd = fopen(build, "rb");
                if (fd)
                {
                    fread(&config.projects[i].builds[config.projects[i].build_count-1].completion, sizeof(time_t), 1, fd);
                    fclose(fd);
                }
                build[build_size] = '\0';

                strcat(build, "/status");
                fd = fopen(build, "rb");
                if (fd)
                {
                    fread(&config.projects[i].builds[config.projects[i].build_count-1].status, sizeof(enum build_status), 1, fd);
                    fclose(fd);
                }
                build[build_size] = '\0';

                free(build);
            }
            closedir(dir);
        }

        // sort the builds
        struct build_t t;
        for (size_t j = 0; j < config.projects[i].build_count; ++j)
        {
            for (size_t k = 0; k < config.projects[i].build_count; ++k)
            {
                struct build_t* a = &config.projects[i].builds[j];
                struct build_t* b = &config.projects[i].builds[k];

                if (atoi(a->name) > atoi(b->name))
                {
                    t = *a;
                    *a = *b;
                    *b = t;
                }
            }
        }

        free(project);
    }

    free(buf);
}

void load_full_build(struct project_t* project, struct build_t* build)
{
    if (!build)
        return;

    char* path = build_dir(project->name, build->name);
    size_t build_size = strlen(path);
    path = realloc(path, (build_size + 11 + 1) * sizeof(char));
    FILE* fd;

    strcat(path, "/log");
    fd = fopen(path, "rb");
    if (fd)
    {
        fseek(fd, 0, SEEK_END);
        size_t size = (size_t)ftell(fd);
        rewind(fd);

        build->log = malloc(size + 1);
        fread(build->log, sizeof(char), size, fd);
        fclose(fd);
        build->log[size] = '\0';
    }
    path[build_size] = '\0';
}

void parse_path(const char* path)
{
    if (path[0] != '/')
        return;

    const char* head = ++path;

    while(*path)
    {
        ++path;
        if (*path == '/' || *path == '\0')
        {
        	assert(path > head);
            size_t len = (size_t)(path - head) + 1;

            if (!len)
                continue;

            ++context.path_length;

            context.path = realloc(context.path, sizeof(char*) * context.path_length);
            assert(context.path);

            context.path[context.path_length-1] = malloc(len * sizeof(char));
            strncpy(context.path[context.path_length-1], head, len-1);
            context.path[context.path_length-1][len-1] = '\0';

            head = path+1;
        }
    }
}

void parse_query(const char* query_arg)
{
    char* query = strdup(query_arg);
    char* head = query;
    char* end = head + strlen(query);

    char* key = head;
    char* value = NULL;

    while (++head <= end)
    {
        if (!value && *head == '=')
        {
            *head = '\0';
            value = head+1;
        }

        if (*head == '&' || *head == '\0')
        {
            *head = '\0';

            if (!strcmp(key, "debug"))
            {
                context.debug = 1;
            }
            else if (!strcmp(key, "token"))
            {
                context.token = strdup(value);
            }
        }
    }

    free(query);
}
