extern "C"
{
#include "../../client/client.h"
    refexport_t re;
}

#include <ILog.h>
#include "gra_local.h"

struct image_s *R_RegisterSkin(char *name);

void refreshExport()
{
    re.api_version = API_VERSION;
    re.Init = [](void *hinstance, void *wndproc) -> qboolean {
        LOGF(LogLevel::eDEBUG, "re.Init()");
        return true;
    };
    re.Shutdown = [](void) { LOGF(LogLevel::eDEBUG, "re.Debug()"); };
    re.BeginRegistration = [](char *map) { LOGF(LogLevel::eDEBUG, "re.BeginRegistration()"); };
    re.RegisterModel = [](char *name) -> model_s * {
        LOGF(LogLevel::eDEBUG, "re.RegisterModel()");
        return NULL;
    };
    re.RegisterSkin = [](char *name) -> image_s * {
        LOGF(LogLevel::eDEBUG, "re.RegisterSkin(%s)", name);
        return R_RegisterSkin(name);
    };
    re.RegisterPic = [](char *name) -> image_s * {
        LOGF(LogLevel::eDEBUG, "re.RegisterPic()");
        return NULL;
    };
    re.SetSky = [](char *name, float rotate, vec3_t axis) { LOGF(LogLevel::eDEBUG, "re.SetSky"); };
    re.EndRegistration = [](void) { LOGF(LogLevel::eDEBUG, "re.EndRegistration"); };
    re.RenderFrame = [](refdef_t *fd) { LOGF(LogLevel::eDEBUG, "re.RenderFrame"); };
    re.DrawGetPicSize = [](int *w, int *h, char *name) { LOGF(LogLevel::eDEBUG, "re.DrawGetPicSize"); };
    re.DrawPic = [](int x, int y, char *name) { LOGF(LogLevel::eDEBUG, "re.DrawPic"); };
    re.DrawStretchPic = [](int x, int y, int w, int h, char *name) { LOGF(LogLevel::eDEBUG, "re.DrawStretchPic"); };
    re.DrawChar = [](int x, int y, int c) { LOGF(LogLevel::eDEBUG, "re.DrawChar"); };
    re.DrawTileClear = [](int x, int y, int w, int h, char *name) { LOGF(LogLevel::eDEBUG, "re.DrawTileClear"); };
    re.DrawFill = [](int x, int y, int w, int h, int c) { LOGF(LogLevel::eDEBUG, "re.DrawFill"); };
    re.DrawFadeScreen = [](void) { LOGF(LogLevel::eDEBUG, "re.DrawFadeScreen"); };
    re.DrawStretchRaw = [](int x, int y, int w, int h, int cols, int rows, byte *data) {
        LOGF(LogLevel::eDEBUG, "re.DrawStretchRaw");
    };
    re.CinematicSetPalette = [](const unsigned char *palette) { LOGF(LogLevel::eDEBUG, "re.CinematicSetPalette"); };
    re.BeginFrame = [](float camera_separation) { LOGF(LogLevel::eDEBUG, "re.BeginFrame"); };
    re.EndFrame = [](void) { LOGF(LogLevel::eDEBUG, "re.EndFrame"); };
    re.EndWorldRenderpass = [](void) { LOGF(LogLevel::eDEBUG, "re.EndWorldRenderPass"); };
    re.AppActivate = [](qboolean activate) { LOGF(LogLevel::eDEBUG, "re.AppActivate"); };
}