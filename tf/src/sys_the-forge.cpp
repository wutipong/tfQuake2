#include <stdio.h>

#include <IApp.h>
#include <IGraphics.h>
#include <IInput.h>
#include <ILog.h>
#include <RingBuffer.h>

#include "sys_event.h"

Renderer *pRenderer = NULL;

Queue *pGraphicsQueue = NULL;
GpuCmdRing gGraphicsCmdRing = {};

SwapChain *pSwapChain = NULL;
RenderTarget *pDepthBuffer = NULL;
Semaphore *pImageAcquiredSemaphore = NULL;

Sampler *pSampler = NULL;

const uint32_t gDataBufferCount = 2;

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

    // window and renderer setup
    RendererDesc settings = {
        .mD3D11Supported = false,
        .mGLESSupported = false,
    };

    initRenderer(GetName(), &settings, &pRenderer);
    // check for init success
    if (!pRenderer)
        return false;

    QueueDesc queueDesc = {
        .mType = QUEUE_TYPE_GRAPHICS,
        .mFlag = QUEUE_FLAG_INIT_MICROPROFILE,
    };
    addQueue(pRenderer, &queueDesc, &pGraphicsQueue);

    GpuCmdRingDesc cmdRingDesc = {
        .pQueue = pGraphicsQueue,
        .mPoolCount = gDataBufferCount,
        .mCmdPerPoolCount = 1,
        .mAddSyncPrimitives = true,
    };
    addGpuCmdRing(pRenderer, &cmdRingDesc, &gGraphicsCmdRing);

    addSemaphore(pRenderer, &pImageAcquiredSemaphore);

    initResourceLoaderInterface(pRenderer);

    SamplerDesc samplerDesc = {
        .mMinFilter = FILTER_NEAREST,
        .mMagFilter = FILTER_NEAREST,
        .mMipMapMode = MIPMAP_MODE_NEAREST,
        .mAddressU = ADDRESS_MODE_CLAMP_TO_EDGE,
        .mAddressV = ADDRESS_MODE_CLAMP_TO_EDGE,
        .mAddressW = ADDRESS_MODE_CLAMP_TO_EDGE,
    };
    addSampler(pRenderer, &samplerDesc, &pSampler);

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

    removeSampler(pRenderer, pSampler);
    removeGpuCmdRing(pRenderer, &gGraphicsCmdRing);
    removeSemaphore(pRenderer, pImageAcquiredSemaphore);

    exitResourceLoaderInterface(pRenderer);

    removeQueue(pRenderer, pGraphicsQueue);

    exitRenderer(pRenderer);
    pRenderer = NULL;
}

bool MainApp::Load(ReloadDesc *pReloadDesc)
{
    return true;
}

void MainApp::Unload(ReloadDesc *pReloadDesc)
{
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