/*
Copyright (C) 1997-2001 Id Software, Inc.
Copyright (C) 2018-2019 Krzysztof Kondrak

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef __GRA_LOCAL_H__
#define __GRA_LOCAL_H__

#include <IGraphics.h>
#include <Math/MathTypes.h>
#include <map>
#include <math.h>
#include <stdio.h>
#include <string>

extern "C"
{
#include "../../client/ref.h"
}

// up / down
#define PITCH 0

// left / right
#define YAW 1

// fall over
#define ROLL 2

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

/*

  skins will be outline flood filled and mip mapped
  pics and sprites with alpha will be outline flood filled
  pic won't be mip mapped

  model skin
  sprite frame
  wall texture
  pic

*/

typedef enum
{
    it_skin,
    it_sprite,
    it_wall,
    it_pic,
    it_sky
} imagetype_t;

typedef struct image_s
{
    std::string name;
    int index;
    imagetype_t type;
    int width, height;               // source image
    int registration_sequence;       // 0 = free
    struct msurface_s *texturechain; // for sort-by-texture world drawing
    Texture *texture;                // Vulkan texture handle

} image_t;

#define MAX_VKTEXTURES 1024

//===================================================================

typedef enum
{
    rserr_ok,

    rserr_invalid_fullscreen,
    rserr_invalid_mode,

    rserr_unknown
} rserr_t;

#include "gra_model.h"

#define MAX_LBM_HEIGHT 480

#define BACKFACE_EPSILON 0.01

//====================================================

extern	image_t		vktextures[MAX_VKTEXTURES];
extern	int			numvktextures;

extern	image_t		*r_notexture;
extern	image_t		*r_particletexture;
extern	entity_t	*currententity;
extern	model_t		*currentmodel;
extern	int			r_visframecount;
extern	int			r_framecount;
extern	cplane_t	frustum[4];
extern	int			c_brush_polys, c_alias_polys;
//
// view origin
//
extern vec3_t vup;
extern vec3_t vpn;
extern vec3_t vright;
extern vec3_t r_origin;

//
// screen size info
//
extern refdef_t r_newrefdef;
extern int r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

extern "C"
{
    extern cvar_t *r_norefresh;
    extern cvar_t *r_lefthand;
    extern cvar_t *r_drawentities;
    extern cvar_t *r_drawworld;
    extern cvar_t *r_speeds;
    extern cvar_t *r_fullbright;
    extern cvar_t *r_novis;
    extern cvar_t *r_nocull;
    extern cvar_t *r_lerpmodels;

    extern cvar_t *r_lightlevel; // FIXME: This is a HACK to get the client's light level

    extern cvar_t *vk_validation;
    extern cvar_t *vk_mode;
    extern cvar_t *vk_log;
    extern cvar_t *vk_picmip;
    extern cvar_t *vk_skymip;
    extern cvar_t *vk_round_down;
    extern cvar_t *vk_flashblend;
    extern cvar_t *vk_finish;
    extern cvar_t *vk_clear;
    extern cvar_t *vk_lockpvs;
    extern cvar_t *vk_polyblend;
    extern cvar_t *vk_modulate;
    extern cvar_t *vk_shadows;
    extern cvar_t *vk_particle_size;
    extern cvar_t *vk_particle_att_a;
    extern cvar_t *vk_particle_att_b;
    extern cvar_t *vk_particle_att_c;
    extern cvar_t *vk_particle_min_size;
    extern cvar_t *vk_particle_max_size;
    extern cvar_t *vk_point_particles;
    extern cvar_t *vk_dynamic;
    extern cvar_t *vk_showtris;
    extern cvar_t *vk_lightmap;
    extern cvar_t *vk_texturemode;
    extern cvar_t *vk_lmaptexturemode;
    extern cvar_t *vk_aniso;
    extern cvar_t *vk_sampleshading;
    extern cvar_t *vk_vsync;
    extern cvar_t *vk_device_idx;
    extern cvar_t *vk_fullscreen_exclusive;

    extern cvar_t *vid_fullscreen;
    extern cvar_t *vid_gamma;

    extern cvar_t *intensity;
}

extern int c_visible_lightmaps;
extern int c_visible_textures;

extern float r_viewproj_matrix[16];

void R_LightPoint(vec3_t p, vec3_t color);
void R_PushDlights(void);

//====================================================================

extern unsigned d_8to24table[256];

extern int registration_sequence;
// extern qvksampler_t vk_current_sampler;
// extern qvksampler_t vk_current_lmap_sampler;

qboolean R_Init(void);
void R_Shutdown(void);

void R_RenderView(refdef_t *fd);
void Vk_ScreenShot_f(void);
void R_DrawAliasModel(entity_t *e);
void R_DrawBrushModel(entity_t *e);
void R_DrawSpriteModel(entity_t *e);
void R_DrawBeam(entity_t *e);
void R_DrawWorld(void);
void R_RenderDlights(void);
void R_DrawAlphaSurfaces(void);
void R_RenderBrushPoly(msurface_t *fa, float *modelMatrix, float alpha);
void R_InitParticleTexture(void);
void Draw_InitLocal(void);
// void Vk_SubdivideSurface(msurface_t *fa);
qboolean R_CullBox(vec3_t mins, vec3_t maxs);
void R_RotateForEntity(entity_t *e, float *mvMatrix);
void R_MarkLeaves(void);

void EmitWaterPolys(msurface_t *fa, image_t *texture, float *modelMatrix, vec4 color);
void R_AddSkySurface(msurface_t *fa);
void R_ClearSkyBox(void);
void R_DrawSkyBox(void);
void R_MarkLights(dlight_t *light, int bit, mnode_t *node);

void COM_StripExtension(char *in, char *out);

void Draw_GetPicSize(int *w, int *h, char *name);
void Draw_Pic(int x, int y, char *name);
void Draw_StretchPic(int x, int y, int w, int h, char *name);
void Draw_Char(int x, int y, int c);
void Draw_TileClear(int x, int y, int w, int h, char *name);
void Draw_Fill(int x, int y, int w, int h, int c);
void Draw_FadeScreen(void);
void Draw_StretchRaw(int x, int y, int w, int h, int cols, int rows, byte *data);

void R_BeginFrame(float camera_separation);
void R_EndFrame(void);
void R_SetPalette(const unsigned char *palette);

struct image_s *R_RegisterSkin(char *name);

void LoadPCX(std::string filename, byte **pic, byte **palette, int *width, int *height);

image_t *GRA_LoadPic(const std::string &name, byte *pic, int width, int height, imagetype_t type, int bits);
image_t *GRA_FindImage(std::string name, imagetype_t type);

void GRA_ImageList_f(void);

void GRA_InitImages(void);
void GRA_ShutdownImages(void);
void GRA_FreeUnusedImages(void);
void Vk_DrawParticles(int n, const particle_t particles[], const unsigned *colortable);

#define MAX_LIGHTMAPS 128
#define DYNLIGHTMAP_OFFSET MAX_LIGHTMAPS

typedef struct
{
    float inverse_intensity;
    bool fullscreen;

    int prev_mode;

    unsigned char *d_16to8table;

    Texture *lightmap_textures[MAX_LIGHTMAPS * 2];

    int currenttextures[2];
    int currenttmu;

    float camera_separation;
    bool stereo_enabled;
} vkstate_t;

extern vkstate_t vk_state;

void Mat_Identity(float *matrix);
void Mat_Mul(float *m1, float *m2, float *res);
void Mat_Translate(float *matrix, float x, float y, float z);
void Mat_Rotate(float *matrix, float deg, float x, float y, float z);
void Mat_Scale(float *matrix, float x, float y, float z);
void Mat_Perspective(float *matrix, float *correction_matrix, float fovy, float aspect, float zNear, float zFar);
void Mat_Ortho(float *matrix, float left, float right, float bottom, float top, float zNear, float zFar);

#endif
