#ifndef GRA_COMMON_H
#define GRA_COMMON_H

#include <IApp.h>

bool GRA_init_graphics(IApp *app);
bool GRA_exit_graphics();


bool GRA_load(ReloadDesc *pReloadDesc);
void GRA_unload(ReloadDesc *pReloadDesc);
#endif