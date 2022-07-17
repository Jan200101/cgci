#include "env.h"
#include "parser.h"
#include "context.h"

struct context_t context;

void init_context()
{
    context.document_root = getenv_default("DOCUMENT_ROOT", "/");
    context.raw_path = getenv_default("PATH_INFO", "/");
    context.path = malloc(1);
    context.path_length = 0;

    context.project = NULL;
    context.action = NULL;
    context.index = NULL;
    context.extra = NULL;

    context.token = NULL;

    context.debug = 0;

    parse_path(context.raw_path);
    parse_query(getenv_default("QUERY_STRING", ""));

    for (size_t i = 0; i < context.path_length; ++i)
    {
        switch(i)
        {
            case 0:
                context.project = context.path[i];
                break;

            case 1:
                context.action = context.path[i];
                break;

            case 2:
                context.index = context.path[i];
                break;

            case 3:
                context.extra = context.path[i];
                break;

            default:
                break;
        }
    }
}

void deinit_context()
{
    if (context.path)
    {
        for (size_t i = 0; i < context.path_length; ++i)
        {
            if (context.path[i])
                free(context.path[i]);
        }

        free(context.path);

        if (context.token)
            free(context.token);
    }
}
