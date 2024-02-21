#ifndef GRA_COMMON_H
#define GRA_COMMON_H

#include <IApp.h>

bool GRA_init_graphics(IApp *app);
bool GRA_exit_graphics();


void GRA_add_shaders();
void GRA_remove_shaders();
#endif