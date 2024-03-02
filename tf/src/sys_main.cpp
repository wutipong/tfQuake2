#include <stdio.h>

#include <IApp.h>
#include <IGraphics.h>
#include <IInput.h>
#include <ILog.h>
#include <ITime.h>
#include <format>
#include <soloud.h>
#include <string>

extern "C"
{
#include "../../client/client.h"

    unsigned int sys_frame_time;
    viddef_t viddef;

    // Console variables that we need to access from this module
    cvar_t *vid_ref;  // Name of Refresh DLL loaded
    cvar_t *vid_xpos; // X coordinate of window position
    cvar_t *vid_ypos; // Y coordinate of window position
    cvar_t *vid_refresh;
    cvar_t *vid_hudscale;
    cvar_t *r_customwidth;
    cvar_t *r_customheight;
    cvar_t *viewsize;
    cvar_t *in_joystick;

    int Sys_Milliseconds(void);
}

#include "gra_common.h"
#include "sys_event.h"

extern Renderer *pRenderer;
extern Queue *pGraphicsQueue;

static std::string _errorMsg;
static bool refresh = false;
static bool isQuit = false;

SoLoud::Soloud gSoloud{};
viddef_t vid;

void refreshSettings()
{
    refresh = true;
}

void refreshExport();

class MainApp : public IApp
{
  public:
    bool Init() override;
    void Exit() override;
    bool Load(ReloadDesc *pReloadDesc) override;
    void Unload(ReloadDesc *pReloadDesc) override;
    void Draw() override;
    void Update(float deltaTime) override;
    const char *GetName() override;
};

const char *MainApp::GetName()
{
    return "tfQuake2";
}

bool MainApp::Init()
{
    gSoloud.init();

    Qcommon_Init(IApp::argc, const_cast<char **>(IApp::argv));
    // FILE PATHS
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_SHADER_BINARIES, "CompiledShaders");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_GPU_CONFIG, "GPUCfg");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_TEXTURES, "Textures");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_FONTS, "Fonts");
    fsSetPathForResourceDir(pSystemFileIO, RM_DEBUG, RD_SCREENSHOTS, "Screenshots");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_SCRIPTS, "Scripts");
    fsSetPathForResourceDir(pSystemFileIO, RM_DEBUG, RD_DEBUG, "Debug");

    GRA_InitGraphics(this);
    refreshExport();
    InputSystemDesc inputDesc = {
        .pRenderer = pRenderer,
        .pWindow = pWindow,
    };

    if (!initInputSystem(&inputDesc))
        return false;

    SYS_RegisterInput();

    return true;
}

void MainApp::Exit()
{
    Com_Quit();
    Qcommon_Shutdown();
    exitInputSystem();

    GRA_ExitGraphics();

    gSoloud.deinit();
}

bool MainApp::Load(ReloadDesc *pReloadDesc)
{
    GRA_Load(pReloadDesc, this);
    return true;
}

void MainApp::Unload(ReloadDesc *pReloadDesc)
{
    GRA_Unload(pReloadDesc);
}

void MainApp::Update(float deltaTime)
{
    updateInputSystem(deltaTime, mSettings.mWidth, mSettings.mHeight);
    
    sys_frame_time = getSystemTime();
    vid.width = mSettings.mWidth;
    vid.height = mSettings.mHeight;

    Sys_Milliseconds();

    Qcommon_Frame(static_cast<int>(deltaTime * 1000));
    if (!_errorMsg.empty())
    {
        LOGF(LogLevel::eERROR, _errorMsg.c_str());
        mSettings.mQuit = true;

        _errorMsg.clear();
    }

    if (isQuit)
    {
        mSettings.mQuit = true;
    }
}

void MainApp::Draw()
{
    GRA_Draw(this);
}

DEFINE_APPLICATION_MAIN(MainApp);

extern "C"
{
    void Sys_Error(char *error, ...)
    {
        va_list argptr;
        
        va_start(argptr, error);
        LOGF(eERROR, argptr);
        va_end(argptr);
        
        isQuit = true;
    }

    char *Sys_GetClipboardData(void)
    {
        return NULL;
    }

    void Sys_SendKeyEvents(void)
    {
    }

    void Sys_AppActivate(void)
    {
    }

    void Sys_Init(void)
    {
    }

    char *Sys_ConsoleInput(void)
    {
        return NULL;
    }

    void Sys_ConsoleOutput(char *string)
    {
        LOGF(LogLevel::eINFO, string);
    }

    void Sys_Quit(void)
    {
        isQuit = true;
    }
}
