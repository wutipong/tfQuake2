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
static void _addRootSignatures();
static void _removeRootSignatures();
static void _addPipelines();
static void _removePipelines();
static void _addSwapChain(IApp *pApp);

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
        _addRootSignatures();
        // addDescriptorSets();
    }
    
    if (pReloadDesc->mType & (RELOAD_TYPE_RESIZE | RELOAD_TYPE_RENDERTARGET))
    {
        _addSwapChain(pApp);
        if (!pSwapChain)
            return false;

        /*
        if (!addDepthBuffer())
            return false;
        */
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
            // removeRenderTarget(pRenderer, pDepthBuffer);
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

void _addRootSignatures()
{
    Shader *shaders[] = {
        drawTexQuadShader,  drawParticlesShader, drawModelShader,  drawSpriteShader, drawPolyShader,
        drawPolyLmapShader, drawPolyWarpShader,  drawSkyboxShader, worldWarpShader,  postprocessShader,
    };
    uint32_t shadersCount = 10;

    const char *pStaticSamplers[] = {"textureSampler"};
    RootSignatureDesc rootDesc = {};
    rootDesc.mStaticSamplerCount = 1;
    rootDesc.ppStaticSamplerNames = pStaticSamplers;
    rootDesc.ppStaticSamplers = &pSampler;
    rootDesc.mShaderCount = shadersCount;
    rootDesc.ppShaders = shaders;
    addRootSignature(pRenderer, &rootDesc, &pRootSignature);
}

static void _removeRootSignatures()
{
    removeRootSignature(pRenderer, pRootSignature);
}

static void _addPipelines()
{
    RasterizerStateDesc rasterizerStateDesc = {};
    rasterizerStateDesc.mCullMode = CULL_MODE_NONE;

    RasterizerStateDesc sphereRasterizerStateDesc = {};
    sphereRasterizerStateDesc.mCullMode = CULL_MODE_FRONT;

    DepthStateDesc depthStateDesc = {};
    depthStateDesc.mDepthTest = true;
    depthStateDesc.mDepthWrite = true;
    depthStateDesc.mDepthFunc = CMP_GEQUAL;

    PipelineDesc desc = {};
    desc.mType = PIPELINE_TYPE_GRAPHICS;

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

    /*
    GraphicsPipelineDesc& pipelineSettings = desc.mGraphicsDesc;
    pipelineSettings.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
    pipelineSettings.mRenderTargetCount = 1;
    pipelineSettings.pDepthState = &depthStateDesc;
    pipelineSettings.pColorFormats = &pSwapChain->ppRenderTargets[0]->mFormat;
    pipelineSettings.mSampleCount = pSwapChain->ppRenderTargets[0]->mSampleCount;
    pipelineSettings.mSampleQuality = pSwapChain->ppRenderTargets[0]->mSampleQuality;
    pipelineSettings.mDepthStencilFormat = pDepthBuffer->mFormat;
    pipelineSettings.pRootSignature = pRootSignature;
    pipelineSettings.pShaderProgram = pSphereShader;
    pipelineSettings.pVertexLayout = &gSphereVertexLayout;
    pipelineSettings.pRasterizerState = &sphereRasterizerStateDesc;
    pipelineSettings.mVRFoveatedRendering = true;
    addPipeline(pRenderer, &desc, &pSpherePipeline);

    if (pRenderer->pGpu->mSettings.mGpuBreadcrumbs)
    {
        pipelineSettings.pShaderProgram = pCrashShader;
        addPipeline(pRenderer, &desc, &pCrashPipeline);
    }

    // layout and pipeline for skybox draw
    VertexLayout vertexLayout = {};
    vertexLayout.mBindingCount = 1;
    vertexLayout.mBindings[0].mStride = sizeof(float4);
    vertexLayout.mAttribCount = 1;
    vertexLayout.mAttribs[0].mSemantic = SEMANTIC_POSITION;
    vertexLayout.mAttribs[0].mFormat = TinyImageFormat_R32G32B32A32_SFLOAT;
    vertexLayout.mAttribs[0].mBinding = 0;
    vertexLayout.mAttribs[0].mLocation = 0;
    vertexLayout.mAttribs[0].mOffset = 0;
    pipelineSettings.pVertexLayout = &vertexLayout;

    pipelineSettings.pDepthState = NULL;
    pipelineSettings.pRasterizerState = &rasterizerStateDesc;
    pipelineSettings.pShaderProgram = pSkyBoxDrawShader; //-V519
    addPipeline(pRenderer, &desc, &pSkyBoxDrawPipeline);

    */

    /*
    // textured quad pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, basic, basic);
     vk_drawTexQuadPipeline.depthTestEnable = VK_FALSE;
     QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRG_RG, &vk_drawTexQuadPipeline, &vk_renderpasses[RP_UI],
    shaders, 2, NULL); QVk_DebugSetObjectName((uint64_t)vk_drawTexQuadPipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT,
    "Pipeline Layout: textured quad"); QVk_DebugSetObjectName((uint64_t)vk_drawTexQuadPipeline.pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: textured quad");

     // draw particles pipeline (using a texture)
     VK_LOAD_VERTFRAG_SHADERS(shaders, particle, basic);
     vk_drawParticlesPipeline.blendOpts.blendEnable = VK_TRUE;
     QVk_CreatePipeline(&vk_samplerDescSetLayout, 1, &vertInfoRGB_RGBA_RG, &vk_drawParticlesPipeline,
    &vk_renderpasses[RP_WORLD], shaders, 2, &pushConstantRangeMatrix);
     QVk_DebugSetObjectName((uint64_t)vk_drawParticlesPipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout:
    textured particles"); QVk_DebugSetObjectName((uint64_t)vk_drawParticlesPipeline.pl, VK_OBJECT_TYPE_PIPELINE,
    "Pipeline: textured particles");

     // draw particles pipeline (using point list)
     VK_LOAD_VERTFRAG_SHADERS(shaders, point_particle, point_particle);
     vk_drawPointParticlesPipeline.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
     vk_drawPointParticlesPipeline.blendOpts.blendEnable = VK_TRUE;
     QVk_CreatePipeline(&vk_uboDescSetLayout, 1, &vertInfoRGB_RGBA, &vk_drawPointParticlesPipeline,
    &vk_renderpasses[RP_WORLD], shaders, 2, &pushConstantRangeMatrix);
     QVk_DebugSetObjectName((uint64_t)vk_drawPointParticlesPipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: point particles"); QVk_DebugSetObjectName((uint64_t)vk_drawPointParticlesPipeline.pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: point particles");

     // colored quad pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, basic_color_quad, basic_color_quad);
     for (int i = 0; i < 2; ++i)
     {
         vk_drawColorQuadPipeline[i].depthTestEnable = VK_FALSE;
         vk_drawColorQuadPipeline[i].blendOpts.blendEnable = VK_TRUE;
         QVk_CreatePipeline(&vk_uboDescSetLayout, 1, &vertInfoRG, &vk_drawColorQuadPipeline[i], &vk_renderpasses[i],
    shaders, 2, NULL);
     }
     QVk_DebugSetObjectName((uint64_t)vk_drawColorQuadPipeline[0].layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: colored quad (RP_WORLD)"); QVk_DebugSetObjectName((uint64_t)vk_drawColorQuadPipeline[0].pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: colored quad (RP_WORLD)");
     QVk_DebugSetObjectName((uint64_t)vk_drawColorQuadPipeline[1].layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: colored quad (RP_UI)"); QVk_DebugSetObjectName((uint64_t)vk_drawColorQuadPipeline[1].pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: colored quad (RP_UI)");

     // untextured null model
     VK_LOAD_VERTFRAG_SHADERS(shaders, nullmodel, basic_color_quad);
     vk_drawNullModelPipeline.cullMode = VK_CULL_MODE_NONE;
     vk_drawNullModelPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
     QVk_CreatePipeline(&vk_uboDescSetLayout, 1, &vertInfoRGB_RGB, &vk_drawNullModelPipeline,
    &vk_renderpasses[RP_WORLD], shaders, 2, &pushConstantRangeMatrix);
     QVk_DebugSetObjectName((uint64_t)vk_drawNullModelPipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout:
    null model"); QVk_DebugSetObjectName((uint64_t)vk_drawNullModelPipeline.pl, VK_OBJECT_TYPE_PIPELINE, "Pipeline: null
    model");

     // textured model
     VK_LOAD_VERTFRAG_SHADERS(shaders, model, model);
     for (int i = 0; i < 2; ++i)
     {
         vk_drawModelPipelineStrip[i].topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
         vk_drawModelPipelineStrip[i].blendOpts.blendEnable = VK_TRUE;
         QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRGB_RGBA_RG, &vk_drawModelPipelineStrip[i],
    &vk_renderpasses[i], shaders, 2, &pushConstantRangeMatrix);

         vk_drawModelPipelineFan[i].topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
         vk_drawModelPipelineFan[i].blendOpts.blendEnable = VK_TRUE;
         QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRGB_RGBA_RG, &vk_drawModelPipelineFan[i],
    &vk_renderpasses[i], shaders, 2, &pushConstantRangeMatrix);
     }
     QVk_DebugSetObjectName((uint64_t)vk_drawModelPipelineStrip[0].layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: draw model: strip (RP_WORLD)"); QVk_DebugSetObjectName((uint64_t)vk_drawModelPipelineStrip[0].pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: draw model: strip (RP_WORLD)");
     QVk_DebugSetObjectName((uint64_t)vk_drawModelPipelineStrip[1].layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: draw model: strip (RP_UI)"); QVk_DebugSetObjectName((uint64_t)vk_drawModelPipelineStrip[1].pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: draw model: strip (RP_UI)");
     QVk_DebugSetObjectName((uint64_t)vk_drawModelPipelineFan[0].layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: draw model: fan (RP_WORLD)"); QVk_DebugSetObjectName((uint64_t)vk_drawModelPipelineFan[0].pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: draw model: fan (RP_WORLD)");
     QVk_DebugSetObjectName((uint64_t)vk_drawModelPipelineFan[1].layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: draw model: fan (RP_UI)"); QVk_DebugSetObjectName((uint64_t)vk_drawModelPipelineFan[1].pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: draw model: fan (RP_UI)");

     // dedicated model pipelines for translucent objects with depth write disabled
     vk_drawNoDepthModelPipelineStrip.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
     vk_drawNoDepthModelPipelineStrip.depthWriteEnable = VK_FALSE;
     vk_drawNoDepthModelPipelineStrip.blendOpts.blendEnable = VK_TRUE;
     QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRGB_RGBA_RG, &vk_drawNoDepthModelPipelineStrip,
    &vk_renderpasses[RP_WORLD], shaders, 2, &pushConstantRangeMatrix);
     QVk_DebugSetObjectName((uint64_t)vk_drawNoDepthModelPipelineStrip.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: translucent model: strip"); QVk_DebugSetObjectName((uint64_t)vk_drawNoDepthModelPipelineStrip.pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: translucent model: strip");

     vk_drawNoDepthModelPipelineFan.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
     vk_drawNoDepthModelPipelineFan.depthWriteEnable = VK_FALSE;
     vk_drawNoDepthModelPipelineFan.blendOpts.blendEnable = VK_TRUE;
     QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRGB_RGBA_RG, &vk_drawNoDepthModelPipelineFan,
    &vk_renderpasses[RP_WORLD], shaders, 2, &pushConstantRangeMatrix);
     QVk_DebugSetObjectName((uint64_t)vk_drawNoDepthModelPipelineFan.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: translucent model: fan"); QVk_DebugSetObjectName((uint64_t)vk_drawNoDepthModelPipelineFan.pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: translucent model: fan");

     // dedicated model pipelines for when left-handed weapon model is drawn
     vk_drawLefthandModelPipelineStrip.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
     vk_drawLefthandModelPipelineStrip.cullMode = VK_CULL_MODE_FRONT_BIT;
     QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRGB_RGBA_RG, &vk_drawLefthandModelPipelineStrip,
    &vk_renderpasses[RP_WORLD], shaders, 2, &pushConstantRangeMatrix);
     QVk_DebugSetObjectName((uint64_t)vk_drawLefthandModelPipelineStrip.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT,
    "Pipeline Layout: left-handed model: strip"); QVk_DebugSetObjectName((uint64_t)vk_drawLefthandModelPipelineStrip.pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: left-handed model: strip");

     vk_drawLefthandModelPipelineFan.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
     vk_drawLefthandModelPipelineFan.cullMode = VK_CULL_MODE_FRONT_BIT;
     QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRGB_RGBA_RG, &vk_drawLefthandModelPipelineFan,
    &vk_renderpasses[RP_WORLD], shaders, 2, &pushConstantRangeMatrix);
     QVk_DebugSetObjectName((uint64_t)vk_drawLefthandModelPipelineFan.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline
    Layout: left-handed model: fan"); QVk_DebugSetObjectName((uint64_t)vk_drawLefthandModelPipelineFan.pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: left-handed model: fan");

     // draw sprite pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, sprite, basic);
     vk_drawSpritePipeline.blendOpts.blendEnable = VK_TRUE;
     QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRGB_RG, &vk_drawSpritePipeline, &vk_renderpasses[RP_WORLD],
    shaders, 2, &pushConstantRangeMatrix); QVk_DebugSetObjectName((uint64_t)vk_drawSpritePipeline.layout,
    VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout: sprite");
     QVk_DebugSetObjectName((uint64_t)vk_drawSpritePipeline.pl, VK_OBJECT_TYPE_PIPELINE, "Pipeline: sprite");

     // draw polygon pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, polygon, basic);
     vk_drawPolyPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
     vk_drawPolyPipeline.blendOpts.blendEnable = VK_TRUE;
     QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRGB_RG, &vk_drawPolyPipeline, &vk_renderpasses[RP_WORLD],
    shaders, 2, &pushConstantRangeMatrix); QVk_DebugSetObjectName((uint64_t)vk_drawPolyPipeline.layout,
    VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout: polygon");
     QVk_DebugSetObjectName((uint64_t)vk_drawPolyPipeline.pl, VK_OBJECT_TYPE_PIPELINE, "Pipeline: polygon");

     // draw lightmapped polygon
     VK_LOAD_VERTFRAG_SHADERS(shaders, polygon_lmap, polygon_lmap);
     vk_drawPolyLmapPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
     QVk_CreatePipeline(samplerUboLmapDsLayouts, 3, &vertInfoRGB_RG_RG, &vk_drawPolyLmapPipeline,
    &vk_renderpasses[RP_WORLD], shaders, 2, &pushConstantRangeMatrix);
     QVk_DebugSetObjectName((uint64_t)vk_drawPolyLmapPipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout:
    lightmapped polygon"); QVk_DebugSetObjectName((uint64_t)vk_drawPolyLmapPipeline.pl, VK_OBJECT_TYPE_PIPELINE,
    "Pipeline: lightmapped polygon");

     // draw polygon with warp effect (liquid) pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, polygon_warp, basic);
     vk_drawPolyWarpPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
     vk_drawPolyWarpPipeline.blendOpts.blendEnable = VK_TRUE;
     QVk_CreatePipeline(samplerUboLmapDsLayouts, 2, &vertInfoRGB_RG, &vk_drawPolyWarpPipeline,
    &vk_renderpasses[RP_WORLD], shaders, 2, &pushConstantRangeMatrix);
     QVk_DebugSetObjectName((uint64_t)vk_drawPolyWarpPipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout:
    warped polygon (liquids)"); QVk_DebugSetObjectName((uint64_t)vk_drawPolyWarpPipeline.pl, VK_OBJECT_TYPE_PIPELINE,
    "Pipeline: warped polygon (liquids)");

     // draw beam pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, beam, basic_color_quad);
     vk_drawBeamPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
     vk_drawBeamPipeline.depthWriteEnable = VK_FALSE;
     vk_drawBeamPipeline.blendOpts.blendEnable = VK_TRUE;
     QVk_CreatePipeline(&vk_uboDescSetLayout, 1, &vertInfoRGB, &vk_drawBeamPipeline, &vk_renderpasses[RP_WORLD],
    shaders, 2, &pushConstantRangeMatrix); QVk_DebugSetObjectName((uint64_t)vk_drawBeamPipeline.layout,
    VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout: beam"); QVk_DebugSetObjectName((uint64_t)vk_drawBeamPipeline.pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: beam");

     // draw skybox pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, skybox, basic);
     QVk_CreatePipeline(samplerUboDsLayouts, 2, &vertInfoRGB_RG, &vk_drawSkyboxPipeline, &vk_renderpasses[RP_WORLD],
    shaders, 2, &pushConstantRangeMatrix); QVk_DebugSetObjectName((uint64_t)vk_drawSkyboxPipeline.layout,
    VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout: skybox");
     QVk_DebugSetObjectName((uint64_t)vk_drawSkyboxPipeline.pl, VK_OBJECT_TYPE_PIPELINE, "Pipeline: skybox");

     // draw dynamic light pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, d_light, basic_color_quad);
     vk_drawDLightPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
     vk_drawDLightPipeline.depthWriteEnable = VK_FALSE;
     vk_drawDLightPipeline.cullMode = VK_CULL_MODE_FRONT_BIT;
     vk_drawDLightPipeline.blendOpts.blendEnable = VK_TRUE;
     vk_drawDLightPipeline.blendOpts.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
     vk_drawDLightPipeline.blendOpts.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
     vk_drawDLightPipeline.blendOpts.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
     vk_drawDLightPipeline.blendOpts.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
     QVk_CreatePipeline(&vk_uboDescSetLayout, 1, &vertInfoRGB_RGB, &vk_drawDLightPipeline, &vk_renderpasses[RP_WORLD],
    shaders, 2, NULL); QVk_DebugSetObjectName((uint64_t)vk_drawDLightPipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT,
    "Pipeline Layout: dynamic light"); QVk_DebugSetObjectName((uint64_t)vk_drawDLightPipeline.pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: dynamic light");

     // vk_showtris render pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, d_light, basic_color_quad);
     vk_showTrisPipeline.cullMode = VK_CULL_MODE_NONE;
     vk_showTrisPipeline.depthTestEnable = VK_FALSE;
     vk_showTrisPipeline.depthWriteEnable = VK_FALSE;
     vk_showTrisPipeline.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
     QVk_CreatePipeline(&vk_uboDescSetLayout, 1, &vertInfoRGB_RGB, &vk_showTrisPipeline, &vk_renderpasses[RP_WORLD],
    shaders, 2, NULL); QVk_DebugSetObjectName((uint64_t)vk_showTrisPipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT,
    "Pipeline Layout: show triangles"); QVk_DebugSetObjectName((uint64_t)vk_showTrisPipeline.pl,
    VK_OBJECT_TYPE_PIPELINE, "Pipeline: show triangles");

     //vk_shadows render pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, shadows, basic_color_quad);
     vk_shadowsPipelineStrip.blendOpts.blendEnable = VK_TRUE;
     vk_shadowsPipelineStrip.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
     QVk_CreatePipeline(&vk_uboDescSetLayout, 1, &vertInfoRGB, &vk_shadowsPipelineStrip, &vk_renderpasses[RP_WORLD],
    shaders, 2, &pushConstantRangeMatrix); QVk_DebugSetObjectName((uint64_t)vk_shadowsPipelineStrip.layout,
    VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout: draw shadows: strip");
     QVk_DebugSetObjectName((uint64_t)vk_shadowsPipelineStrip.pl, VK_OBJECT_TYPE_PIPELINE, "Pipeline: draw shadows:
    strip");

     vk_shadowsPipelineFan.blendOpts.blendEnable = VK_TRUE;
     vk_shadowsPipelineFan.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
     QVk_CreatePipeline(&vk_uboDescSetLayout, 1, &vertInfoRGB, &vk_shadowsPipelineFan, &vk_renderpasses[RP_WORLD],
    shaders, 2, &pushConstantRangeMatrix); QVk_DebugSetObjectName((uint64_t)vk_shadowsPipelineFan.layout,
    VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout: draw shadows: fan");
     QVk_DebugSetObjectName((uint64_t)vk_shadowsPipelineFan.pl, VK_OBJECT_TYPE_PIPELINE, "Pipeline: draw shadows: fan");

     // underwater world warp pipeline (postprocess)
     VK_LOAD_VERTFRAG_SHADERS(shaders, world_warp, world_warp);
     vk_worldWarpPipeline.depthTestEnable = VK_FALSE;
     vk_worldWarpPipeline.depthWriteEnable = VK_FALSE;
     vk_worldWarpPipeline.cullMode = VK_CULL_MODE_NONE;
     QVk_CreatePipeline(&vk_samplerDescSetLayout, 1, &vertInfoNull, &vk_worldWarpPipeline,
    &vk_renderpasses[RP_WORLD_WARP], shaders, 2, &pushConstantRangeWorldWarpFrag);
     QVk_DebugSetObjectName((uint64_t)vk_worldWarpPipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout:
    underwater view warp"); QVk_DebugSetObjectName((uint64_t)vk_worldWarpPipeline.pl, VK_OBJECT_TYPE_PIPELINE,
    "Pipeline: underwater view warp");

     // postprocessing pipeline
     VK_LOAD_VERTFRAG_SHADERS(shaders, postprocess, postprocess);
     vk_postprocessPipeline.depthTestEnable = VK_FALSE;
     vk_postprocessPipeline.depthWriteEnable = VK_FALSE;
     vk_postprocessPipeline.cullMode = VK_CULL_MODE_NONE;
     QVk_CreatePipeline(&vk_samplerDescSetLayout, 1, &vertInfoNull, &vk_postprocessPipeline, &vk_renderpasses[RP_UI],
    shaders, 2, &pushConstantRangePostprocessFrag); QVk_DebugSetObjectName((uint64_t)vk_postprocessPipeline.layout,
    VK_OBJECT_TYPE_PIPELINE_LAYOUT, "Pipeline Layout: world postprocess");
     QVk_DebugSetObjectName((uint64_t)vk_postprocessPipeline.pl, VK_OBJECT_TYPE_PIPELINE, "Pipeline: world
    postprocess");

    */
}

static void _removePipelines()
{
    /*
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
    */
}

static void _addSwapChain(IApp *pApp)
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
}