#include "gra_common.h"

#include <IGraphics.h>
#include <RingBuffer.h>

const uint32_t gDataBufferCount = 2;

Renderer *pRenderer = NULL;

Queue *pGraphicsQueue = NULL;
GpuCmdRing gGraphicsCmdRing = {};

SwapChain *pSwapChain = NULL;
RenderTarget *pDepthBuffer = NULL;
Semaphore *pImageAcquiredSemaphore = NULL;

Sampler *pSampler = NULL;

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

static void _addShaders();
static void _removeShaders();
static bool _addRootSignatures();
static void _removeRootSignatures();
static void _addPipelines();
static void _removePipelines();
static bool _addSwapChain(IApp *pApp);
static bool _addDepthBuffer(IApp *pApp);

bool GRA_init_graphics(IApp *app)
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

    SamplerDesc samplerDesc = {
        .mMinFilter = FILTER_NEAREST,
        .mMagFilter = FILTER_NEAREST,
        .mMipMapMode = MIPMAP_MODE_NEAREST,
        .mAddressU = ADDRESS_MODE_CLAMP_TO_EDGE,
        .mAddressV = ADDRESS_MODE_CLAMP_TO_EDGE,
        .mAddressW = ADDRESS_MODE_CLAMP_TO_EDGE,
    };
    addSampler(pRenderer, &samplerDesc, &pSampler);

    return true;
}

bool GRA_exit_graphics()
{
    removeSampler(pRenderer, pSampler);
    removeGpuCmdRing(pRenderer, &gGraphicsCmdRing);
    removeSemaphore(pRenderer, pImageAcquiredSemaphore);

    exitResourceLoaderInterface(pRenderer);

    removeQueue(pRenderer, pGraphicsQueue);

    exitRenderer(pRenderer);
    pRenderer = NULL;

    return true;
}

bool GRA_load(ReloadDesc *pReloadDesc, IApp *pApp)
{
    if (pReloadDesc->mType & RELOAD_TYPE_SHADER)
    {
        _addShaders();
        if (!_addRootSignatures())
        {
            return false;
        }
        // addDescriptorSets();
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
    }
    /*
        prepareDescriptorSets();

        UserInterfaceLoadDesc uiLoad = {};
        uiLoad.mColorFormat = pSwapChain->ppRenderTargets[0]->mFormat;
        uiLoad.mHeight = mSettings.mHeight;
        uiLoad.mWidth = mSettings.mWidth;
        uiLoad.mLoadType = pReloadDesc->mType;
        loadUserInterface(&uiLoad);

        FontSystemLoadDesc fontLoad = {};
        fontLoad.mColorFormat = pSwapChain->ppRenderTargets[0]->mFormat;
        fontLoad.mHeight = mSettings.mHeight;
        fontLoad.mWidth = mSettings.mWidth;
        fontLoad.mLoadType = pReloadDesc->mType;
        loadFontSystem(&fontLoad);

        initScreenshotInterface(pRenderer, pGraphicsQueue);
        */
    return true;
}
void GRA_unload(ReloadDesc *pReloadDesc)
{
    waitQueueIdle(pGraphicsQueue);

    if (pReloadDesc->mType & (RELOAD_TYPE_SHADER | RELOAD_TYPE_RENDERTARGET))
    {
        _removePipelines();
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
        // removeDescriptorSets();
        _removeRootSignatures();
        _removeShaders();
    }
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