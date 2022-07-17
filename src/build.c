#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "config.h"
#include "fs.h"
#include "build.h"

static void write_build(char* project, char* build_id, struct build_t* build)
{
    char* build_path = build_dir(project, build_id);
    if (!build_path)
        return;

    if (!isDir(build_path))
        makeDir(build_path);

    size_t build_size = strlen(build_path);
    build_path = realloc(build_path, (build_size + 11 + 1) * sizeof(char));

    FILE* fd;

    strcat(build_path, "/timestamp");
    fd = fopen(build_path, "wb");
    if (fd)
    {
        fwrite(&build->timestamp, sizeof(time_t), 1, fd);
        fclose(fd);
    }
    build_path[build_size] = '\0';

    strcat(build_path, "/completion");
    fd = fopen(build_path, "wb");
    if (fd)
    {
        fwrite(&build->completion, sizeof(build->completion), 1, fd);
        fclose(fd);
    }
    build_path[build_size] = '\0';

    strcat(build_path, "/status");
    fd = fopen(build_path, "wb");
    if (fd)
    {
        fwrite(&build->status, sizeof(build->status), 1, fd);
        fclose(fd);
    }
    build_path[build_size] = '\0';

    free(build_path);

}

void create_build()
{
    if (!current_project)
        return;

    int build_id = 0;

    if (current_project->build_count > 0)
        build_id = atoi(current_project->builds[0].name)+1;

    if (build_id <= 0)
        build_id = 1;

    struct build_t build;
    build.name = NULL;
    build.timestamp = time(NULL);
    build.completion = 0;
    build.status = STATUS_INPROGRESS;

    size_t name_len = 1;
    int temp = build_id;

    while (temp > 10)
    {
        temp /= 10;
        name_len++;
    }

    build.name = malloc((name_len + 1) * sizeof(char));
    sprintf(build.name, "%d", build_id);

    write_build(current_project->name, build.name, &build);

    if (!fork())
    {
        char* build_path = build_dir(current_project->name, build.name);
        if (build_path)
        {
            makeDir(build_path);
            build_path = realloc(build_path, (strlen(build_path) + 4 + 1) * sizeof(char));
            strcat(build_path, "/log");

            freopen(build_path, "w", stdout);
            freopen(build_path, "w", stderr);

            free(build_path);
        }

        int status = system(current_project->script_path);
        if (status)
            build.status = STATUS_FAILURE;
        else
            build.status = STATUS_SUCCESS;

        build.completion = time(NULL);

        write_build(current_project->name, build.name, &build);
        exit(status);
    }
}