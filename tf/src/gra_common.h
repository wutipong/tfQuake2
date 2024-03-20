#ifndef GRA_COMMON_H
#define GRA_COMMON_H

#include "gra_local.h"

#include <IApp.h>
#include <IGraphics.h>
#include <RingBuffer.h>
#include <IProfiler.h>

#include <array>

constexpr uint32_t gDataBufferCount = 2;

enum class RenderPass
{
	WORLD = 0,      // renders game world to offscreen buffer
	UI = 1,         // render UI elements and game console
	WORLD_WARP = 2, // perform postprocessing on RP_WORLD (underwater screen warp)
	COUNT = 3
};

bool GRA_InitGraphics(IApp *app);
bool GRA_ExitGraphics();

bool GRA_Load(ReloadDesc *pReloadDesc, IApp *pApp);
void GRA_Unload(ReloadDesc *pReloadDesc);
void GRA_Draw(IApp *pApp);

void GRA_BindUniformBuffer(Cmd *pCmd, void* uniform, uint32_t size);
uint32_t GRA_BindTriangleFanIBO(Cmd *pCmd, uint32_t count);
void GRA_BindVertexBuffer(Cmd *pCmd, void *data, uint32_t size, uint32_t stride);

void GRA_DrawColorRect(vec2 offset, vec2 scale, vec4 color, RenderPass rpType);
void GRA_DrawTexRect(vec2 offset, vec2 scale, vec2 uvOffset, vec2 uvScale, image_t *texture);

extern Renderer *pRenderer;
extern Sampler *pSampler;
extern image_t vktextures[MAX_VKTEXTURES];

extern Queue *pGraphicsQueue;
extern GpuCmdRing gGraphicsCmdRing;

extern SwapChain *pSwapChain ;
extern Semaphore *pImageAcquiredSemaphore;
extern ProfileToken gGpuProfileToken;
extern int gFrameIndex;

extern RenderTarget *pRenderTarget;
extern RenderTarget *pDepthBuffer;
extern RenderTarget *pWorldRenderTarget;
extern RenderTarget *pWorldWarpRenderTarget;

// render pipelines
extern Pipeline *drawTexQuadPipeline;
extern Pipeline *drawColorQuadPipeline[2];
extern Pipeline *drawModelPipelineStrip[2];
extern Pipeline *drawModelPipelineFan[2];
extern Pipeline *drawNoDepthModelPipelineStrip;
extern Pipeline *drawNoDepthModelPipelineFan;
extern Pipeline *drawLefthandModelPipelineStrip;
extern Pipeline *drawLefthandModelPipelineFan;
extern Pipeline *drawNullModelPipeline;
extern Pipeline *drawParticlesPipeline;
extern Pipeline *drawPointParticlesPipeline;
extern Pipeline *drawSpritePipeline;
extern Pipeline *drawPolyPipeline;
extern Pipeline *drawPolyLmapPipeline;
extern Pipeline *drawPolyWarpPipeline;
extern Pipeline *drawBeamPipeline;
extern Pipeline *drawSkyboxPipeline;
extern Pipeline *drawDLightPipeline;
extern Pipeline *showTrisPipeline;
extern Pipeline *shadowsPipelineStrip;
extern Pipeline *shadowsPipelineFan;
extern Pipeline *worldWarpPipeline;
extern Pipeline *postprocessPipeline;

extern RootSignature *pRootSignature;

extern std::array<GPURingBuffer, gDataBufferCount> dynamicUniformBuffers;
extern std::array<GPURingBuffer, gDataBufferCount> dynamicVertexBuffers;
extern std::array<GPURingBuffer, gDataBufferCount> dynamicIndexBuffers;

extern Cmd *pCmd;

extern DescriptorSet *pDSTexture[MAX_VKTEXTURES];
extern DescriptorSet *pDSDynamicUniforms;
extern DescriptorSet *pDSLightMap[MAX_LIGHTMAPS * 2];
extern DescriptorSet *pDSWorldTexture;
extern DescriptorSet *pDSWorldWarpTexture;
extern DescriptorSet *pDSUniform;

extern Buffer *pBufferTexRectVbo;
extern Buffer *pBufferColorRectVbo;
extern Buffer *pBufferRectIbo;
extern Buffer *pBufferUniform;

extern uint32_t gPushConstantSmall;
extern uint32_t gPushConstantLarge;
extern uint32_t gPushConstantPolygonWarp;

extern ProfileToken gGpuProfileToken;

extern IApp* pApp;

extern bool vk_frameStarted;
#endif