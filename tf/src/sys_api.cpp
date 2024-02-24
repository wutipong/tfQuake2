
extern "C"
{
    #include "../../client/client.h"
    // static HINSTANCE game_library;

    void *Sys_GetGameAPI(void *parms)
    {
        //TODO: implements load API from dll/so/whatsoever in the cross-platform manner
        return NULL;
//         void *(*GetGameAPI)(void *);
//         char name[MAX_OSPATH];
//         char *path;
//         char cwd[MAX_OSPATH];
// #if defined _M_IX86
//         const char *gamename = "gamex86.dll";

// #ifdef NDEBUG
//         const char *debugdir = "release";
// #else
//         const char *debugdir = "debug";
// #endif

// #elif defined _M_X64
//         const char *gamename = "gamex64.dll";

// #ifdef NDEBUG
//         const char *debugdir = "releasex64";
// #else
//         const char *debugdir = "debugx64";
// #endif

// #elif defined _M_ALPHA
//         const char *gamename = "gameaxp.dll";

// #ifdef NDEBUG
//         const char *debugdir = "releaseaxp";
// #else
//         const char *debugdir = "debugaxp";
// #endif

// #endif

//         if (game_library)
//             Com_Error(ERR_FATAL, "Sys_GetGameAPI without Sys_UnloadingGame");

//         // check the current debug directory first for development purposes
//         _getcwd(cwd, sizeof(cwd));
//         Com_sprintf(name, sizeof(name), "%s/%s/%s", cwd, debugdir, gamename);
//         game_library = LoadLibrary(name);
//         if (game_library)
//         {
//             Com_DPrintf("LoadLibrary (%s)\n", name);
//         }
//         else
//         {
// #ifdef DEBUG
//             // check the current directory for other development purposes
//             Com_sprintf(name, sizeof(name), "%s/%s", cwd, gamename);
//             game_library = LoadLibrary(name);
//             if (game_library)
//             {
//                 Com_DPrintf("LoadLibrary (%s)\n", name);
//             }
//             else
// #endif
//             {
//                 // now run through the search paths
//                 path = NULL;
//                 while (1)
//                 {
//                     path = FS_NextPath(path);
//                     if (!path)
//                         return NULL; // couldn't find one anywhere
//                     Com_sprintf(name, sizeof(name), "%s/%s", path, gamename);
//                     game_library = LoadLibrary(name);
//                     if (game_library)
//                     {
//                         Com_DPrintf("LoadLibrary (%s)\n", name);
//                         break;
//                     }
//                 }
//             }
//         }

//         GetGameAPI = (void *)GetProcAddress(game_library, "GetGameAPI");
//         if (!GetGameAPI)
//         {
//             Sys_UnloadGame();
//             return NULL;
//         }

//         return GetGameAPI(parms);
    }

    void Sys_UnloadGame(void)
    {
        // if (!FreeLibrary(game_library))
        //     Com_Error(ERR_FATAL, "FreeLibrary failed for game library");
        // game_library = NULL;
    }
}