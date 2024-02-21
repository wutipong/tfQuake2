#include <stdio.h>

#include <IApp.h>
#include <IGraphics.h>
#include <IInput.h>
#include <ILog.h>

#include "gra_common.h"
#include "sys_event.h"

extern Renderer *pRenderer;
extern Queue *pGraphicsQueue;

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
    // FILE PATHS
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_SHADER_BINARIES, "CompiledShaders");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_GPU_CONFIG, "GPUCfg");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_TEXTURES, "Textures");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_FONTS, "Fonts");
    fsSetPathForResourceDir(pSystemFileIO, RM_DEBUG, RD_SCREENSHOTS, "Screenshots");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_SCRIPTS, "Scripts");
    fsSetPathForResourceDir(pSystemFileIO, RM_DEBUG, RD_DEBUG, "Debug");

    GRA_init_graphics(this);

    InputSystemDesc inputDesc = {
        .pRenderer = pRenderer,
        .pWindow = pWindow,
    };

    if (!initInputSystem(&inputDesc))
        return false;

    SYS_register_input();

    return true;
}

void MainApp::Exit()
{
    exitInputSystem();

    GRA_exit_graphics();
}

bool MainApp::Load(ReloadDesc *pReloadDesc)
{
    if (pReloadDesc->mType & RELOAD_TYPE_SHADER)
    {
        GRA_add_shaders();
        // addRootSignatures();
        // addDescriptorSets();
    }
    return true;
}

void MainApp::Unload(ReloadDesc *pReloadDesc)
{
    waitQueueIdle(pGraphicsQueue);
    /*

    if (pReloadDesc->mType & (RELOAD_TYPE_SHADER | RELOAD_TYPE_RENDERTARGET))
    {
        removePipelines();
        removeResource(pSphereVertexBuffer);
        removeResource(pSphereIndexBuffer);
    }

    if (pReloadDesc->mType & (RELOAD_TYPE_RESIZE | RELOAD_TYPE_RENDERTARGET))
    {
        removeSwapChain(pRenderer, pSwapChain);
        removeRenderTarget(pRenderer, pDepthBuffer);
    }
    */

    if (pReloadDesc->mType & RELOAD_TYPE_SHADER)
    {
        // removeDescriptorSets();
        // removeRootSignatures();
        GRA_remove_shaders();
    }
}

void MainApp::Update(float deltaTime)
{
    updateInputSystem(deltaTime, mSettings.mWidth, mSettings.mHeight);
    // mSettings.mQuit = true;
}

void MainApp::Draw()
{
}

DEFINE_APPLICATION_MAIN(MainApp);