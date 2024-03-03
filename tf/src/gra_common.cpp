#include "gra_common.h"

#include <IFont.h>
#include <IGraphics.h>
#include <IProfiler.h>
#include <IScreenshot.h>
#include <IUI.h>
#include <RingBuffer.h>

const uint32_t gDataBufferCount = 2;

Renderer *pRenderer = NULL;

Queue *pGraphicsQueue = NULL;
GpuCmdRing gGraphicsCmdRing = {};

SwapChain *pSwapChain = NULL;
RenderTarget *pDepthBuffer = NULL;
Semaphore *pImageAcquiredSemaphore = NULL;
ProfileToken gGpuProfileToken = PROFILE_INVALID_TOKEN;
int gFrameIndex = 0;
FontDrawDesc gFrameTimeDraw = {};
uint32_t gFontID = 0;
extern Sampler *pSampler = NULL;

// render pipelines
Pipeline *drawTexQuadPipeline;
Pipeline *drawColorQuadPipeline[2];
Pipeline *drawModelPipelineStrip[2];
Pipeline *drawModelPipelineFan[2];
Pipeline *drawNoDepthModelPipelineStrip;
Pipeline *drawNoDepthModelPipelineFan;
Pipeline *drawLefthandModelPipelineStrip;
Pipeline *drawLefthandModelPipelineFan;
Pipeline *drawNullModelPipeline;
Pipeline *drawParticlesPipeline;
Pipeline *drawPointParticlesPipeline;
Pipeline *drawSpritePipeline;
Pipeline *drawPolyPipeline;
Pipeline *drawPolyLmapPipeline;
Pipeline *drawPolyWarpPipeline;
Pipeline *drawBeamPipeline;
Pipeline *drawSkyboxPipeline;
Pipeline *drawDLightPipeline;
Pipeline *showTrisPipeline;
Pipeline *shadowsPipelineStrip;
Pipeline *shadowsPipelineFan;
Pipeline *worldWarpPipeline;
Pipeline *postprocessPipeline;

// Shaders
Shader *drawTexQuadShader;
Shader *drawColorQuadShader;
Shader *drawModelShader;
Shader *drawNullModelShader;
Shader *drawParticlesShader;
Shader *drawPointParticlesShader;
Shader *drawSpriteShader;
Shader *drawPolyShader;
Shader *drawPolyLmapShader;
Shader *drawPolyWarpShader;
Shader *drawBeamShader;
Shader *drawSkyboxShader;
Shader *drawDLightShader;
Shader *shadowsShader;
Shader *worldWarpShader;
Shader *postprocessShader;

RootSignature *pRootSignature = NULL;

GPURingBuffer dynamicUniformBuffer;

Cmd *pCmd;

DescriptorSet *pDescriptorSetTexture = {NULL};
DescriptorSet *pDescriptorSetUniforms = {NULL};

Buffer* texRectVbo;
Buffer* colorRectVbo;
Buffer* rectIbo;

static void _addShaders();
static void _removeShaders();
static bool _addRootSignatures();
static void _removeRootSignatures();
static void _addPipelines();
static void _removePipelines();
static bool _addSwapChain(IApp *pApp);
static bool _addDepthBuffer(IApp *pApp);
static bool _addDescriptorSets();
static bool _removeDescriptorSets();
static void _addStaticBuffers();
static void _removeStaticBuffers();


bool GRA_InitGraphics(IApp *app)
{
    // window and renderer setup
    RendererDesc settings = {
        .mD3D11Supported = false,
        .mGLESSupported = false,
    };

    initRenderer(app->GetName(), &settings, &pRenderer);
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
    addUniformGPURingBuffer(pRenderer, 65536, &dynamicUniformBuffer, true);

    SamplerDesc samplerDesc = {
        .mMinFilter = FILTER_NEAREST,
        .mMagFilter = FILTER_NEAREST,
        .mMipMapMode = MIPMAP_MODE_NEAREST,
        .mAddressU = ADDRESS_MODE_CLAMP_TO_EDGE,
        .mAddressV = ADDRESS_MODE_CLAMP_TO_EDGE,
        .mAddressW = ADDRESS_MODE_CLAMP_TO_EDGE,
    };
    addSampler(pRenderer, &samplerDesc, &pSampler);

    FontSystemDesc fontRenderDesc = {};
    fontRenderDesc.pRenderer = pRenderer;
    if (!initFontSystem(&fontRenderDesc))
        return false; // report?

    // Initialize Forge User Interface Rendering
    UserInterfaceDesc uiRenderDesc = {};
    uiRenderDesc.pRenderer = pRenderer;
    initUserInterface(&uiRenderDesc);

    // Initialize micro profiler and its UI.
    ProfilerDesc profiler = {};
    profiler.pRenderer = pRenderer;
    profiler.mWidthUI = app->mSettings.mWidth;
    profiler.mHeightUI = app->mSettings.mHeight;
    initProfiler(&profiler);

    gGpuProfileToken = addGpuProfiler(pRenderer, pGraphicsQueue, "Graphics");

    return true;
}

bool GRA_ExitGraphics()
{
    exitProfiler();
    exitUserInterface();
    exitFontSystem();

    removeSampler(pRenderer, pSampler);
    removeGpuCmdRing(pRenderer, &gGraphicsCmdRing);
    removeGPURingBuffer(&dynamicUniformBuffer);
    removeSemaphore(pRenderer, pImageAcquiredSemaphore);

    exitResourceLoaderInterface(pRenderer);

    removeQueue(pRenderer, pGraphicsQueue);

    exitRenderer(pRenderer);
    pRenderer = NULL;

    return true;
}

bool GRA_Load(ReloadDesc *pReloadDesc, IApp *pApp)
{
    if (pReloadDesc->mType & RELOAD_TYPE_SHADER)
    {
        _addShaders();
        if (!_addRootSignatures())
        {
            return false;
        }
        _addDescriptorSets();
    }

    if (pReloadDesc->mType & (RELOAD_TYPE_RESIZE | RELOAD_TYPE_RENDERTARGET))
    {
        if (!_addSwapChain(pApp))
        {
            return false;
        }

        if (!_addDepthBuffer(pApp))
        {
            return false;
        }
    }

    if (pReloadDesc->mType & (RELOAD_TYPE_SHADER | RELOAD_TYPE_RENDERTARGET))
    {
        _addPipelines();
        _addStaticBuffers();
    }

    //    prepareDescriptorSets();

    UserInterfaceLoadDesc uiLoad = {};
    uiLoad.mColorFormat = pSwapChain->ppRenderTargets[0]->mFormat;
    uiLoad.mHeight = pApp->mSettings.mHeight;
    uiLoad.mWidth = pApp->mSettings.mWidth;
    uiLoad.mLoadType = pReloadDesc->mType;
    loadUserInterface(&uiLoad);

    FontSystemLoadDesc fontLoad = {};
    fontLoad.mColorFormat = pSwapChain->ppRenderTargets[0]->mFormat;
    fontLoad.mHeight = pApp->mSettings.mHeight;
    fontLoad.mWidth = pApp->mSettings.mWidth;
    fontLoad.mLoadType = pReloadDesc->mType;
    loadFontSystem(&fontLoad);

    initScreenshotInterface(pRenderer, pGraphicsQueue);

    return true;
}
void GRA_Unload(ReloadDesc *pReloadDesc)
{
    waitQueueIdle(pGraphicsQueue);

    unloadFontSystem(pReloadDesc->mType);
    unloadUserInterface(pReloadDesc->mType);

    if (pReloadDesc->mType & (RELOAD_TYPE_SHADER | RELOAD_TYPE_RENDERTARGET))
    {
        _removePipelines();
        _removeStaticBuffers();
        // removeResource(pSphereVertexBuffer);
        // removeResource(pSphereIndexBuffer);
    }

    if (pReloadDesc->mType & (RELOAD_TYPE_RESIZE | RELOAD_TYPE_RENDERTARGET))
    {
        removeSwapChain(pRenderer, pSwapChain);
        removeRenderTarget(pRenderer, pDepthBuffer);
    }

    if (pReloadDesc->mType & RELOAD_TYPE_SHADER)
    {
        _removeDescriptorSets();
        _removeRootSignatures();
        _removeShaders();
    }

    exitScreenshotInterface();
}

void _addShaders()
{
    ShaderLoadDesc desc = {};
    desc.mStages[0].pFileName = "basic.vert";
    desc.mStages[1].pFileName = "basic.frag";

    addShader(pRenderer, &desc, &drawTexQuadShader);

    desc = {};
    desc.mStages[0].pFileName = "particle.vert";
    desc.mStages[1].pFileName = "basic.frag";

    addShader(pRenderer, &desc, &drawParticlesShader);

    desc = {};
    desc.mStages[0].pFileName = "point_particle.vert";
    desc.mStages[1].pFileName = "point_particle.frag";

    addShader(pRenderer, &desc, &drawPointParticlesShader);

    desc = {};
    desc.mStages[0].pFileName = "basic_color_quad.vert";
    desc.mStages[1].pFileName = "basic_color_quad.frag";

    addShader(pRenderer, &desc, &drawColorQuadShader);

    desc = {};
    desc.mStages[0].pFileName = "nullmodel.vert";
    desc.mStages[1].pFileName = "basic_color_quad.frag";

    addShader(pRenderer, &desc, &drawNullModelShader);

    desc = {};
    desc.mStages[0].pFileName = "model.vert";
    desc.mStages[1].pFileName = "model.frag";

    addShader(pRenderer, &desc, &drawModelShader);

    desc = {};
    desc.mStages[0].pFileName = "sprite.vert";
    desc.mStages[1].pFileName = "basic.frag";

    addShader(pRenderer, &desc, &drawSpriteShader);

    desc = {};
    desc.mStages[0].pFileName = "polygon.vert";
    desc.mStages[1].pFileName = "basic.frag";

    addShader(pRenderer, &desc, &drawPolyShader);

    desc = {};
    desc.mStages[0].pFileName = "polygon_lmap.vert";
    desc.mStages[1].pFileName = "polygon_lmap.frag";

    addShader(pRenderer, &desc, &drawPolyLmapShader);

    desc = {};
    desc.mStages[0].pFileName = "polygon_warp.vert";
    desc.mStages[1].pFileName = "basic.frag";

    addShader(pRenderer, &desc, &drawPolyWarpShader);

    desc = {};
    desc.mStages[0].pFileName = "beam.vert";
    desc.mStages[1].pFileName = "basic_color_quad.frag";

    addShader(pRenderer, &desc, &drawBeamShader);

    desc = {};
    desc.mStages[0].pFileName = "skybox.vert";
    desc.mStages[1].pFileName = "basic.frag";

    addShader(pRenderer, &desc, &drawSkyboxShader);

    desc = {};
    desc.mStages[0].pFileName = "d_light.vert";
    desc.mStages[1].pFileName = "basic_color_quad.frag";

    addShader(pRenderer, &desc, &drawDLightShader);

    desc = {};
    desc.mStages[0].pFileName = "shadows.vert";
    desc.mStages[1].pFileName = "basic_color_quad.frag";

    addShader(pRenderer, &desc, &shadowsShader);

    desc = {};
    desc.mStages[0].pFileName = "world_warp.vert";
    desc.mStages[1].pFileName = "world_warp.frag";

    addShader(pRenderer, &desc, &worldWarpShader);

    desc = {};
    desc.mStages[0].pFileName = "postprocess.vert";
    desc.mStages[1].pFileName = "postprocess.frag";

    addShader(pRenderer, &desc, &postprocessShader);
}

void _removeShaders()
{
    removeShader(pRenderer, drawTexQuadShader);
    removeShader(pRenderer, drawColorQuadShader);
    removeShader(pRenderer, drawModelShader);
    removeShader(pRenderer, drawNullModelShader);
    removeShader(pRenderer, drawParticlesShader);
    removeShader(pRenderer, drawPointParticlesShader);
    removeShader(pRenderer, drawSpriteShader);
    removeShader(pRenderer, drawPolyShader);
    removeShader(pRenderer, drawPolyLmapShader);
    removeShader(pRenderer, drawPolyWarpShader);
    removeShader(pRenderer, drawBeamShader);
    removeShader(pRenderer, drawSkyboxShader);
    removeShader(pRenderer, drawDLightShader);
    removeShader(pRenderer, shadowsShader);
    removeShader(pRenderer, worldWarpShader);
    removeShader(pRenderer, postprocessShader);
}

bool _addRootSignatures()
{
    Shader *shaders[] = {
        drawTexQuadShader,        drawColorQuadShader, drawModelShader,  drawNullModelShader, drawParticlesShader,
        drawPointParticlesShader, drawSpriteShader,    drawPolyShader,   drawPolyLmapShader,  drawPolyWarpShader,
        drawBeamShader,           drawSkyboxShader,    drawDLightShader, shadowsShader,       worldWarpShader,
        postprocessShader,
    };
    uint32_t shadersCount = 16;

    const char *pStaticSamplers[] = {"textureSampler"};

    RootSignatureDesc rootDesc = {
        .ppShaders = shaders,
        .mShaderCount = shadersCount,
        .ppStaticSamplerNames = pStaticSamplers,
        .ppStaticSamplers = &pSampler,
        .mStaticSamplerCount = 1,
    };

    addRootSignature(pRenderer, &rootDesc, &pRootSignature);

    return pRootSignature != NULL;
}

static void _removeRootSignatures()
{
    removeRootSignature(pRenderer, pRootSignature);
}

static void _addPipelines()
{
    VertexLayout vertexLayoutF2Pos = {
        .mBindings =
            {
                {.mStride = sizeof(float2)},
            },
        .mAttribs =
            {
                {
                    .mSemantic = SEMANTIC_POSITION,
                    .mFormat = TinyImageFormat_R32G32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 0,
                    .mOffset = 0,
                },
            },
        .mBindingCount = 1,
        .mAttribCount = 1,
    };

    VertexLayout vertexLayoutF2PosTexcoord = {
        .mBindings =
            {
                {.mStride = sizeof(float2)},
                {.mStride = sizeof(float2)},
            },
        .mAttribs =
            {
                {
                    .mSemantic = SEMANTIC_POSITION,
                    .mFormat = TinyImageFormat_R32G32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 0,
                    .mOffset = 0,
                },
                {
                    .mSemantic = SEMANTIC_TEXCOORD0,
                    .mFormat = TinyImageFormat_R32G32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 1,
                    .mOffset = sizeof(float2),
                },
            },
        .mBindingCount = 1,
        .mAttribCount = 2,
    };

    VertexLayout vertexLayoutF3Pos = {
        .mBindings =
            {
                {.mStride = sizeof(float3)},
            },
        .mAttribs =
            {
                {
                    .mSemantic = SEMANTIC_POSITION,
                    .mFormat = TinyImageFormat_R32G32B32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 0,
                    .mOffset = 0,
                },
            },
        .mBindingCount = 1,
        .mAttribCount = 1,
    };

    VertexLayout vertexLayoutF3PosTexcoord = {
        .mBindings =
            {
                {.mStride = sizeof(float3)},
                {.mStride = sizeof(float2)},
            },
        .mAttribs =
            {
                {
                    .mSemantic = SEMANTIC_POSITION,
                    .mFormat = TinyImageFormat_R32G32B32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 0,
                    .mOffset = 0,
                },
                {
                    .mSemantic = SEMANTIC_TEXCOORD0,
                    .mFormat = TinyImageFormat_R32G32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 1,
                    .mOffset = sizeof(float3),
                },
            },
        .mBindingCount = 1,
        .mAttribCount = 2,
    };

    VertexLayout vertexLayoutF3PosF3Color = {
        .mBindings =
            {
                {.mStride = sizeof(float3)},
                {.mStride = sizeof(float3)},
            },
        .mAttribs =
            {
                {
                    .mSemantic = SEMANTIC_POSITION,
                    .mFormat = TinyImageFormat_R32G32B32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 0,
                    .mOffset = 0,
                },
                {
                    .mSemantic = SEMANTIC_COLOR,
                    .mFormat = TinyImageFormat_R32G32B32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 1,
                    .mOffset = sizeof(float3),
                },
            },
        .mBindingCount = 1,
        .mAttribCount = 2,
    };

    VertexLayout vertexLayoutF3PosF4Color = {
        .mBindings =
            {
                {.mStride = sizeof(float3)},
                {.mStride = sizeof(float4)},
            },
        .mAttribs =
            {
                {
                    .mSemantic = SEMANTIC_POSITION,
                    .mFormat = TinyImageFormat_R32G32B32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 0,
                    .mOffset = 0,
                },
                {
                    .mSemantic = SEMANTIC_COLOR,
                    .mFormat = TinyImageFormat_R32G32B32A32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 1,
                    .mOffset = sizeof(float3),
                },
            },
        .mBindingCount = 1,
        .mAttribCount = 2,
    };

    VertexLayout vertexLayoutF3PosTexcoordTexcoord = {
        .mBindings =
            {
                {.mStride = sizeof(float3)},
                {.mStride = sizeof(float2)},
                {.mStride = sizeof(float2)},
            },
        .mAttribs =
            {
                {
                    .mSemantic = SEMANTIC_POSITION,
                    .mFormat = TinyImageFormat_R32G32B32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 0,
                    .mOffset = 0,
                },
                {
                    .mSemantic = SEMANTIC_TEXCOORD0,
                    .mFormat = TinyImageFormat_R32G32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 1,
                    .mOffset = sizeof(float3),
                },
                {
                    .mSemantic = SEMANTIC_TEXCOORD1,
                    .mFormat = TinyImageFormat_R32G32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 2,
                    .mOffset = sizeof(float3) + sizeof(float2),
                },
            },
        .mBindingCount = 1,
        .mAttribCount = 3,
    };

    VertexLayout vertexLayoutF3PosF4ColorTexcoord = {
        .mBindings =
            {
                {.mStride = sizeof(float3)},
                {.mStride = sizeof(float4)},
                {.mStride = sizeof(float2)},
            },
        .mAttribs =
            {
                {
                    .mSemantic = SEMANTIC_POSITION,
                    .mFormat = TinyImageFormat_R32G32B32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 0,
                    .mOffset = 0,
                },
                {
                    .mSemantic = SEMANTIC_COLOR,
                    .mFormat = TinyImageFormat_R32G32B32A32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 1,
                    .mOffset = sizeof(float3),
                },
                {
                    .mSemantic = SEMANTIC_TEXCOORD0,
                    .mFormat = TinyImageFormat_R32G32_SFLOAT,
                    .mBinding = 0,
                    .mLocation = 2,
                    .mOffset = sizeof(float3) + sizeof(float4),
                },
            },
        .mBindingCount = 1,
        .mAttribCount = 3,
    };

    RasterizerStateDesc rasterizerStateCullBackDesc = {
        .mCullMode = CULL_MODE_BACK,
    };

    RasterizerStateDesc rasterizerStateCullFrontDesc = {
        .mCullMode = CULL_MODE_FRONT,
    };

    RasterizerStateDesc rasterizerStateCullNoneDesc = {
        .mCullMode = CULL_MODE_NONE,
    };

    DepthStateDesc depthStateDesc = {
        .mDepthTest = true,
        .mDepthWrite = true,
        .mDepthFunc = CMP_GEQUAL,
    };

    BlendStateDesc blendStateDesc = {
        .mSrcFactors = {BC_SRC_ALPHA},
        .mDstFactors = {BC_ONE_MINUS_SRC_ALPHA},
        .mSrcAlphaFactors = {BC_ONE},
        .mDstAlphaFactors = {BC_ZERO},
        .mBlendModes = {BM_ADD},
        .mBlendAlphaModes = {BM_ADD},
        .mColorWriteMasks = {COLOR_MASK_ALL},
        .mRenderTargetMask = BLEND_STATE_TARGET_0,
        .mIndependentBlend = false,
    };

    PipelineDesc initDesc = {
        .mGraphicsDesc =
            {
                .pRootSignature = pRootSignature,
                .pDepthState = &depthStateDesc,
                .pRasterizerState = &rasterizerStateCullBackDesc,
                .pColorFormats = &pSwapChain->ppRenderTargets[0]->mFormat,
                .mRenderTargetCount = 1,
                .mSampleCount = pSwapChain->ppRenderTargets[0]->mSampleCount,
                .mSampleQuality = pSwapChain->ppRenderTargets[0]->mSampleQuality,
                .mDepthStencilFormat = pDepthBuffer->mFormat,
                .mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST,
            },
        .mType = PIPELINE_TYPE_GRAPHICS,
    };

    PipelineDesc desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawTexQuadShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF2PosTexcoord;
    desc.mGraphicsDesc.pDepthState = NULL;

    addPipeline(pRenderer, &desc, &drawTexQuadPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawParticlesShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF4ColorTexcoord;
    desc.mGraphicsDesc.pDepthState = NULL;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;

    addPipeline(pRenderer, &desc, &drawParticlesPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawPointParticlesShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF4Color;
    desc.mGraphicsDesc.pDepthState = NULL;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_POINT_LIST,

    addPipeline(pRenderer, &desc, &drawPointParticlesPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawColorQuadShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF2Pos;
    desc.mGraphicsDesc.pDepthState = NULL;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_POINT_LIST,

    addPipeline(pRenderer, &desc, &drawColorQuadPipeline[0]);
    addPipeline(pRenderer, &desc, &drawColorQuadPipeline[1]);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawNullModelShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF3Color;
    desc.mGraphicsDesc.pRasterizerState = &rasterizerStateCullNoneDesc;

    addPipeline(pRenderer, &desc, &drawNullModelPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawModelShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF4ColorTexcoord;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_STRIP;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;

    addPipeline(pRenderer, &desc, &drawModelPipelineStrip[0]);
    addPipeline(pRenderer, &desc, &drawModelPipelineStrip[1]);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawModelShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF4ColorTexcoord;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;

    addPipeline(pRenderer, &desc, &drawModelPipelineFan[0]);
    addPipeline(pRenderer, &desc, &drawModelPipelineFan[1]);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawModelShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF4ColorTexcoord;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_STRIP;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;
    desc.mGraphicsDesc.pDepthState = NULL;

    addPipeline(pRenderer, &desc, &drawNoDepthModelPipelineStrip);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawModelShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF4ColorTexcoord;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;
    desc.mGraphicsDesc.pDepthState = NULL;

    addPipeline(pRenderer, &desc, &drawNoDepthModelPipelineFan);

    desc = initDesc;
    desc.mGraphicsDesc.pRasterizerState = &rasterizerStateCullFrontDesc;
    desc.mGraphicsDesc.pShaderProgram = drawModelShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF4ColorTexcoord;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_STRIP;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;

    addPipeline(pRenderer, &desc, &drawLefthandModelPipelineStrip);

    desc = initDesc;
    desc.mGraphicsDesc.pRasterizerState = &rasterizerStateCullFrontDesc;
    desc.mGraphicsDesc.pShaderProgram = drawModelShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF4ColorTexcoord;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;

    addPipeline(pRenderer, &desc, &drawLefthandModelPipelineFan);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawSpriteShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosTexcoord;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;

    addPipeline(pRenderer, &desc, &drawSpritePipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawPolyShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosTexcoord;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;

    addPipeline(pRenderer, &desc, &drawPolyPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawPolyLmapShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosTexcoordTexcoord;

    addPipeline(pRenderer, &desc, &drawPolyLmapPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawPolyWarpShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosTexcoord;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;

    addPipeline(pRenderer, &desc, &drawPolyWarpPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawBeamShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3Pos;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_STRIP;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;
    desc.mGraphicsDesc.pDepthState = NULL;

    addPipeline(pRenderer, &desc, &drawBeamPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawSkyboxShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosTexcoord;

    addPipeline(pRenderer, &desc, &drawSkyboxPipeline);

    BlendStateDesc blendStateDLightDesc = {
        .mSrcFactors = {BC_ONE},
        .mDstFactors = {BC_ONE},
        .mSrcAlphaFactors = {BC_ONE},
        .mDstAlphaFactors = {BC_ONE},
    };

    desc = initDesc;
    desc.mGraphicsDesc.pBlendState = &blendStateDLightDesc;
    desc.mGraphicsDesc.pShaderProgram = drawDLightShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF3Color;
    desc.mGraphicsDesc.pDepthState = NULL;
    desc.mGraphicsDesc.pRasterizerState = &rasterizerStateCullFrontDesc;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;

    addPipeline(pRenderer, &desc, &drawDLightPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = drawDLightShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF3Color;
    desc.mGraphicsDesc.pDepthState = NULL;
    desc.mGraphicsDesc.pRasterizerState = &rasterizerStateCullFrontDesc;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_LINE_LIST;

    addPipeline(pRenderer, &desc, &showTrisPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = shadowsShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF3Color;
    desc.mGraphicsDesc.pDepthState = NULL;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_STRIP;

    addPipeline(pRenderer, &desc, &shadowsPipelineStrip);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = shadowsShader;
    desc.mGraphicsDesc.pVertexLayout = &vertexLayoutF3PosF3Color;
    desc.mGraphicsDesc.pDepthState = NULL;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;

    addPipeline(pRenderer, &desc, &shadowsPipelineFan);

    VertexLayout vertexLayout = {};

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = worldWarpShader;
    desc.mGraphicsDesc.pVertexLayout = NULL;
    desc.mGraphicsDesc.pDepthState = NULL;
    desc.mGraphicsDesc.pBlendState = &blendStateDesc;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;

    addPipeline(pRenderer, &desc, &worldWarpPipeline);

    desc = initDesc;
    desc.mGraphicsDesc.pShaderProgram = postprocessShader;
    desc.mGraphicsDesc.pVertexLayout = NULL;
    desc.mGraphicsDesc.pDepthState = NULL;
    desc.mGraphicsDesc.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;

    addPipeline(pRenderer, &desc, &postprocessPipeline);
}

static void _removePipelines()
{
    removePipeline(pRenderer, drawTexQuadPipeline);
    removePipeline(pRenderer, drawColorQuadPipeline[0]);
    removePipeline(pRenderer, drawColorQuadPipeline[1]);
    removePipeline(pRenderer, drawModelPipelineStrip[0]);
    removePipeline(pRenderer, drawModelPipelineStrip[1]);
    removePipeline(pRenderer, drawModelPipelineFan[0]);
    removePipeline(pRenderer, drawModelPipelineFan[1]);
    removePipeline(pRenderer, drawNoDepthModelPipelineStrip);
    removePipeline(pRenderer, drawNoDepthModelPipelineFan);
    removePipeline(pRenderer, drawLefthandModelPipelineStrip);
    removePipeline(pRenderer, drawLefthandModelPipelineFan);
    removePipeline(pRenderer, drawNullModelPipeline);
    removePipeline(pRenderer, drawParticlesPipeline);
    removePipeline(pRenderer, drawPointParticlesPipeline);
    removePipeline(pRenderer, drawSpritePipeline);
    removePipeline(pRenderer, drawPolyPipeline);
    removePipeline(pRenderer, drawPolyLmapPipeline);
    removePipeline(pRenderer, drawPolyWarpPipeline);
    removePipeline(pRenderer, drawBeamPipeline);
    removePipeline(pRenderer, drawSkyboxPipeline);
    removePipeline(pRenderer, drawDLightPipeline);
    removePipeline(pRenderer, showTrisPipeline);
    removePipeline(pRenderer, shadowsPipelineStrip);
    removePipeline(pRenderer, shadowsPipelineFan);
    removePipeline(pRenderer, worldWarpPipeline);
    removePipeline(pRenderer, postprocessPipeline);
}

static bool _addSwapChain(IApp *pApp)
{
    SwapChainDesc swapChainDesc = {
        .mWindowHandle = pApp->pWindow->handle,
        .ppPresentQueues = &pGraphicsQueue,
        .mPresentQueueCount = 1,
        .mImageCount = getRecommendedSwapchainImageCount(pRenderer, &pApp->pWindow->handle),
        .mWidth = (uint32_t)pApp->mSettings.mWidth,
        .mHeight = (uint32_t)pApp->mSettings.mHeight,
        .mColorFormat = getSupportedSwapchainFormat(pRenderer, &swapChainDesc, COLOR_SPACE_SDR_SRGB),
        .mFlags = SWAP_CHAIN_CREATION_FLAG_ENABLE_FOVEATED_RENDERING_VR,
        .mEnableVsync = pApp->mSettings.mVSyncEnabled,
        .mColorSpace = COLOR_SPACE_SDR_SRGB,
    };
    addSwapChain(pRenderer, &swapChainDesc, &pSwapChain);

    return pSwapChain != NULL;
}

static bool _addDepthBuffer(IApp *pApp)
{
    // Add depth buffer
    RenderTargetDesc depthRT = {};
    depthRT.mArraySize = 1;
    depthRT.mClearValue.depth = 0.0f;
    depthRT.mClearValue.stencil = 0;
    depthRT.mDepth = 1;
    depthRT.mFormat = TinyImageFormat_D32_SFLOAT;
    depthRT.mStartState = RESOURCE_STATE_DEPTH_WRITE;
    depthRT.mHeight = pApp->mSettings.mHeight;
    depthRT.mSampleCount = SAMPLE_COUNT_1;
    depthRT.mSampleQuality = 0;
    depthRT.mWidth = pApp->mSettings.mWidth;
    depthRT.mFlags = TEXTURE_CREATION_FLAG_ON_TILE | TEXTURE_CREATION_FLAG_VR_MULTIVIEW;
    addRenderTarget(pRenderer, &depthRT, &pDepthBuffer);

    return pDepthBuffer != NULL;
}

void GRA_Draw(IApp *pApp)
{
    if (pSwapChain->mEnableVsync != pApp->mSettings.mVSyncEnabled)
    {
        waitQueueIdle(pGraphicsQueue);
        ::toggleVSync(pRenderer, &pSwapChain);
    }

    uint32_t swapchainImageIndex;
    acquireNextImage(pRenderer, pSwapChain, pImageAcquiredSemaphore, NULL, &swapchainImageIndex);

    RenderTarget *pRenderTarget = pSwapChain->ppRenderTargets[swapchainImageIndex];
    GpuCmdRingElement elem = getNextGpuCmdRingElement(&gGraphicsCmdRing, true, 1);

    // Stall if CPU is running "gDataBufferCount" frames ahead of GPU
    FenceStatus fenceStatus;
    getFenceStatus(pRenderer, elem.pFence, &fenceStatus);
    if (fenceStatus == FENCE_STATUS_INCOMPLETE)
    {
        waitForFences(pRenderer, 1, &elem.pFence);
    }

    // Reset cmd pool for this frame
    resetCmdPool(pRenderer, elem.pCmdPool);

    Cmd *cmd = elem.pCmds[0];
    beginCmd(cmd);

    cmdBeginGpuFrameProfile(cmd, gGpuProfileToken);

    RenderTargetBarrier barriers[] = {
        {pRenderTarget, RESOURCE_STATE_PRESENT, RESOURCE_STATE_RENDER_TARGET},
    };
    cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 1, barriers);

    // simply record the screen cleaning command
    BindRenderTargetsDesc bindRenderTargets = {};
    bindRenderTargets.mRenderTargetCount = 1;
    bindRenderTargets.mRenderTargets[0] = {pRenderTarget, LOAD_ACTION_CLEAR};
    bindRenderTargets.mDepthStencil = {pDepthBuffer, LOAD_ACTION_CLEAR};
    cmdBindRenderTargets(cmd, &bindRenderTargets);
    cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
    cmdSetScissor(cmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);

    const uint32_t skyboxVbStride = sizeof(float) * 4;

    pCmd = cmd;

    /************************************************************************/
    // draw objects
    /************************************************************************/

    pCmd = NULL;

    cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "Draw UI");

    gFrameTimeDraw.mFontColor = 0xff00ffff;
    gFrameTimeDraw.mFontSize = 18.0f;
    gFrameTimeDraw.mFontID = gFontID;
    float2 txtSizePx = cmdDrawCpuProfile(cmd, float2(8.f, 15.f), &gFrameTimeDraw);
    cmdDrawGpuProfile(cmd, float2(8.f, txtSizePx.y + 75.f), gGpuProfileToken, &gFrameTimeDraw);

    cmdDrawUserInterface(cmd);

    cmdBindRenderTargets(cmd, NULL);
    cmdEndGpuTimestampQuery(cmd, gGpuProfileToken);

    barriers[0] = {pRenderTarget, RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_PRESENT};
    cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 1, barriers);

    cmdEndGpuFrameProfile(cmd, gGpuProfileToken);

    endCmd(cmd);

    FlushResourceUpdateDesc flushUpdateDesc = {};
    flushUpdateDesc.mNodeIndex = 0;
    flushResourceUpdates(&flushUpdateDesc);
    Semaphore *waitSemaphores[2] = {flushUpdateDesc.pOutSubmittedSemaphore, pImageAcquiredSemaphore};

    QueueSubmitDesc submitDesc = {};
    submitDesc.mCmdCount = 1;
    submitDesc.mSignalSemaphoreCount = 1;
    submitDesc.mWaitSemaphoreCount = TF_ARRAY_COUNT(waitSemaphores);
    submitDesc.ppCmds = &cmd;
    submitDesc.ppSignalSemaphores = &elem.pSemaphore;
    submitDesc.ppWaitSemaphores = waitSemaphores;
    submitDesc.pSignalFence = elem.pFence;
    queueSubmit(pGraphicsQueue, &submitDesc);
    QueuePresentDesc presentDesc = {};
    presentDesc.mIndex = swapchainImageIndex;
    presentDesc.mWaitSemaphoreCount = 1;
    presentDesc.pSwapChain = pSwapChain;
    presentDesc.ppWaitSemaphores = &elem.pSemaphore;
    presentDesc.mSubmitDone = true;

    queuePresent(pGraphicsQueue, &presentDesc);
    flipProfiler();

    gFrameIndex = (gFrameIndex + 1) % gDataBufferCount;
}

bool _addDescriptorSets()
{
    DescriptorSetDesc desc = {pRootSignature, DESCRIPTOR_UPDATE_FREQ_NONE, 1};
    addDescriptorSet(pRenderer, &desc, &pDescriptorSetTexture);
    desc = {pRootSignature, DESCRIPTOR_UPDATE_FREQ_PER_FRAME, gDataBufferCount * 2};
    addDescriptorSet(pRenderer, &desc, &pDescriptorSetUniforms);

    return true;
}

bool _removeDescriptorSets()
{
    removeDescriptorSet(pRenderer, pDescriptorSetTexture);
    removeDescriptorSet(pRenderer, pDescriptorSetUniforms);

    return true;
}

static void _addStaticBuffers()
{
	const float texVerts[] = {	-1., -1., 0., 0.,
								 1.,  1., 1., 1.,
								-1.,  1., 0., 1.,
								 1., -1., 1., 0. };

	const float colorVerts[] = { -1., -1.,
								  1.,  1.,
								 -1.,  1.,
								  1., -1. };

	const uint32_t indices[] = { 0, 1, 2, 0, 3, 1 };

    BufferLoadDesc desc = {};
    desc.mDesc.mDescriptors = DESCRIPTOR_TYPE_VERTEX_BUFFER;
    desc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
    desc.mDesc.mSize = sizeof(texVerts);
    desc.pData = texVerts;
    desc.ppBuffer = &texRectVbo;
    addResource(&desc, nullptr);

    desc = {};
    desc.mDesc.mDescriptors = DESCRIPTOR_TYPE_VERTEX_BUFFER;
    desc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
    desc.mDesc.mSize = sizeof(colorVerts);
    desc.pData = colorVerts;
    desc.ppBuffer = &colorRectVbo;
    addResource(&desc, nullptr);

    desc = {};
    desc.mDesc.mDescriptors = DESCRIPTOR_TYPE_VERTEX_BUFFER;
    desc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
    desc.mDesc.mSize = sizeof(indices);
    desc.pData = indices;
    desc.ppBuffer = &rectIbo;
    addResource(&desc, nullptr);
}

static void _removeStaticBuffers()
{
    removeResource(texRectVbo);
    removeResource(colorRectVbo);
    removeResource(rectIbo);
}

void GRA_DrawColorRect(float *ubo, size_t uboSize, RenderPass rpType)
{
    GPURingBufferOffset uniformBlock = getGPURingBufferOffset(&dynamicUniformBuffer, uboSize);
    BufferUpdateDesc updateDesc = {uniformBlock.pBuffer, uniformBlock.mOffset};

    beginUpdateResource(&updateDesc);
    memcpy(updateDesc.pMappedData, ubo, uboSize);
    endUpdateResource(&updateDesc);

    DescriptorDataRange range = {(uint32_t)uniformBlock.mOffset, uboSize};
    DescriptorData params[1] = {};
    params[0].pName = "uniformBlock_rootcbv";
    params[0].ppBuffers = &uniformBlock.pBuffer;
    params[0].pRanges = &range;

    const uint32_t stride = sizeof(float) * 2;

    cmdBindPipeline(pCmd, drawColorQuadPipeline[static_cast<size_t>(rpType)]);
    cmdBindDescriptorSetWithRootCbvs(pCmd, 0, pDescriptorSetUniforms, 1, params);
    cmdBindVertexBuffer(pCmd, 1, &colorRectVbo, &stride, 0);
    cmdBindIndexBuffer(pCmd, rectIbo, INDEX_TYPE_UINT32, 0);

    cmdDrawIndexed(pCmd, 6, 0, 0);
}