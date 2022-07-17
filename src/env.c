#include <stdlib.h>
#include "env.h"

char* getenv_default(const char* name, char* default_val)
{
    char* val = getenv(name);
    if (!val)
        val = default_val;

    return val;

}
