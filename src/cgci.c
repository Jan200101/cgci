#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "config.h"
#include "env.h"
#include "fs.h"
#include "parser.h"
#include "context.h"
#include "ui.h"
#include "cgci.h"

void init()
{
    unsetenv("HOME");
    unsetenv("USER");

    init_context();
    init_config();
}

void deinit()
{
    deinit_context();
    deinit_config();
}

int main(int argc, char **argv, char** envp)
{
    if (argc > 1)
    {
        argv_to_path(argc, argv);
    }

    init();

    if (context.project && !strcmp(context.project, "assets"))
    {
        print_asset(context.action);
    }
    else
    {
        print_html();
    }

    deinit();

    return 0;
}
