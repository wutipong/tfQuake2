#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>

static cvar_t *Cvar_Get (const char *var_name, const char *value, int flags)
{
    return Cvar_Get (std::string(var_name).data(), std::string(value).data(), flags);
}

#endif