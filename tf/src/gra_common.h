#ifndef GRA_COMMON_H
#define GRA_COMMON_H

#include "gra_local.h"

#include <IApp.h>
#include <IGraphics.h>
#include <RingBuffer.h>
#include <IProfiler.h>

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

uint32_t GRA_BindTriangleFanIBO(Cmd *pCmd, uint32_t count);

void GRA_DrawColorRect(float *ubo, size_t uboSize, RenderPass rpType);
void GRA_DrawTexRect(float *ubo, size_t uboSize, image_t *texture);

extern Renderer *pRenderer;
extern Sampler *pSampler;
extern image_t vktextures[MAX_VKTEXTURES];

extern RenderTarget *pRenderTarget;
extern RenderTarget *pDepthBuffer;
extern RenderTarget *pWorldRenderTarget;

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

extern GPURingBuffer dynamicUniformBuffer;
extern GPURingBuffer dynamicVertexBuffer;
extern GPURingBuffer dynamicIndexBuffer;

extern Cmd *pCmd;

extern DescriptorSet *pDescriptorSetsTexture[MAX_VKTEXTURES];
extern DescriptorSet *pDescriptorSetUniforms;
extern DescriptorSet *pDescriptorSetsLightMap[MAX_LIGHTMAPS * 2];

extern Buffer *texRectVbo;
extern Buffer *colorRectVbo;
extern Buffer *rectIbo;

extern uint32_t gPushConstant;

extern ProfileToken gGpuProfileToken;

#endif