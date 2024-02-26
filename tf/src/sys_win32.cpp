#ifdef WIN32

#include <ILog.h>
#include <format>

extern "C"
{
#include "../../client/client.h"
#include <wtypes.h>

    static HINSTANCE game_library;

    void *Sys_GetGameAPI(void *parms)
    {
        void *(*GetGameAPI)(void *);
        const char *gamename = "gamex64.dll";

        if (game_library)
        {
            LOGF(LogLevel::eERROR, "Sys_GetGameAPI without Sys_UnloadingGame");
            return NULL;
        }

        // now run through the search paths
        char *path = NULL;
        while (1)
        {
            path = FS_NextPath(path);
            if (!path)
                return NULL; // couldn't find one anywhere

            std::string name = std::format("{}/{}", path, gamename);
            game_library = LoadLibrary(name.c_str());
            if (game_library)
            {
                LOGF(LogLevel::eINFO, "LoadLibrary (%s)", name.c_str());
                break;
            }
        }

        GetGameAPI = (decltype(GetGameAPI))GetProcAddress(game_library, "GetGameAPI");
        if (!GetGameAPI)
        {
            Sys_UnloadGame();
            return NULL;
        }
        return GetGameAPI(parms);
    }

    void Sys_UnloadGame(void)
    {
        if (!FreeLibrary(game_library))
        {
            LOGF(LogLevel::eERROR, "FreeLibrary failed for game library");
        }
        game_library = NULL;
    }
}

#endif