#ifndef GRA_COMMON_H
#define GRA_COMMON_H

#include <IApp.h>


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

void GRA_DrawColorRect(float *ubo, size_t uboSize, RenderPass rpType);
#endif