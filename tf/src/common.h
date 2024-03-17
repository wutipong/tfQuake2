#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>

static cvar_t *Cvar_Get(const char *var_name, const char *value, int flags)
{
    return Cvar_Get(std::string(var_name).data(), std::string(value).data(), flags);
}

static cvar_t *Cvar_Set(std::string var_name, std::string value)
{
    return Cvar_Set(var_name.data(), value.data());
}

static void Cvar_SetValue(std::string var_name, float value)
{
    return Cvar_SetValue(var_name.data(), value);
}

#endif