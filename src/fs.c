#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>
#endif

#include "fs.h"

#ifdef _WIN32
#define mkdir(path, perm) mkdir(path)
#endif

static struct stat getStat(const char* path)
{
    // fill with 0s by default in the case stat fails
    struct stat sb = {0};

    // the return value signifies if stat failes (e.g. file not found)
    // unimportant for us if it fails it won't touch sb
    stat(path, &sb);

    return sb;
}

int isFile(const char* path)
{
    struct stat sb = getStat(path);

#ifndef _WIN32
    if (S_ISLNK(sb.st_mode))
    {
        char buf[PATH_MAX];
        readlink(path, buf, sizeof(buf));

        return isFile(buf);
    }
#endif
    return S_ISREG(sb.st_mode);
}

int isDir(const char* path)
{
    struct stat sb = getStat(path);

#ifndef _WIN32
    if (S_ISLNK(sb.st_mode))
    {
        char buf[PATH_MAX];
        readlink(path, buf, sizeof(buf));

        return isDir(buf);
    }
#endif
    return S_ISDIR(sb.st_mode);
}

int makeDir(const char* path)
{
    char pathcpy[PATH_MAX];
    char *index;

    strncpy(pathcpy, path, PATH_MAX-1); // make a mutable copy of the path

    for(index = pathcpy+1; *index; ++index)
    {

        if (*index == '/')
        {
            *index = '\0';

            if (mkdir(pathcpy, 0755) != 0)
            {
                if (errno != EEXIST)
                    return -1;
            }

            *index = '/';
        }
    }

    return mkdir(path, 0755);
}

int removeDir(const char* path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;

        r = 0;
        while (!r && (p = readdir(d))) {
            char *buf;
            size_t len;

            // Skip the names "." and ".." as we don't want to recurse on them.
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf) {
                struct stat statbuf = {0};

                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r = removeDir(buf);
#ifndef _WIN32
                    else if (S_ISLNK(statbuf.st_mode))
                        r = unlink(buf);
#endif
                    else
                        r = remove(buf);
                }
                else // it is very likely that we found a dangling symlink which is not detected by stat
                {
                    r = unlink(buf);
                }
                free(buf);
            }
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
}