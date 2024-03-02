#ifndef GRA_COMMON_H
#define GRA_COMMON_H

#include <IApp.h>

bool GRA_InitGraphics(IApp *app);
bool GRA_ExitGraphics();


bool GRA_Load(ReloadDesc *pReloadDesc, IApp *pApp);
void GRA_Unload(ReloadDesc *pReloadDesc);
void GRA_Draw(IApp *pApp);
#endif