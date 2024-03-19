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
// vk_rmain.c
#include "gra_common.h"
#include "gra_local.h"

extern "C"
{
#include "../../client/client.h"
}

#include "common.h"
#include <IFont.h>
#include <IUI.h>

extern viddef_t vid;

refimport_t ri;
refexport_t re;

model_t *r_worldmodel;

vkstate_t vk_state;

image_t *r_notexture;       // use for bad textures
image_t *r_particletexture; // little dot for particles

entity_t *currententity;
model_t *currentmodel;

cplane_t frustum[4];

int r_visframecount; // bumped when going to a new PVS
int r_framecount;    // used for dlight push checking

int c_brush_polys, c_alias_polys;

float v_blend[4]; // final blending color

void Vk_Strings_f(void);
void Vk_PollRestart_f(void);
void Vk_Mem_f(void);

//
// view origin
//
vec3_t vup;
vec3_t vpn;
vec3_t vright;
vec3_t r_origin;

mat4 r_projection_matrix;
float r_proj_aspect;
float r_proj_fovx;
float r_proj_fovy;
mat4 r_view_matrix;
mat4 r_viewproj_matrix;
// correction matrix for perspective in Vulkan
static mat4 r_vulkan_correction = {
    {1.f, 0.f, 0.f, 0.f},
    {0.f, -1.f, 0.f, 0.f},
    {0.f, 0.f, .5f, 0.f},
    {0.f, 0.f, .5f, 1.f},
};
//
// screen size info
//
refdef_t r_newrefdef;

int r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

cvar_t *r_norefresh;
cvar_t *r_drawentities;
cvar_t *r_drawworld;
cvar_t *r_speeds;
cvar_t *r_fullbright;
cvar_t *r_novis;
cvar_t *r_nocull;
cvar_t *r_lerpmodels;
cvar_t *r_lefthand;

cvar_t *r_lightlevel; // FIXME: This is a HACK to get the client's light level

cvar_t *vk_validation;
cvar_t *vk_mode;
cvar_t *vk_log;
cvar_t *vk_picmip;
cvar_t *vk_skymip;
cvar_t *vk_round_down;
cvar_t *vk_flashblend;
cvar_t *vk_finish;
cvar_t *vk_clear;
cvar_t *vk_lockpvs;
cvar_t *vk_polyblend;
cvar_t *vk_modulate;
cvar_t *vk_shadows;
cvar_t *vk_particle_size;
cvar_t *vk_particle_att_a;
cvar_t *vk_particle_att_b;
cvar_t *vk_particle_att_c;
cvar_t *vk_particle_min_size;
cvar_t *vk_particle_max_size;
cvar_t *vk_point_particles;
cvar_t *vk_postprocess;
cvar_t *vk_underwater;
cvar_t *vk_dynamic;
cvar_t *vk_msaa;
cvar_t *vk_showtris;
cvar_t *vk_lightmap;
cvar_t *vk_texturemode;
cvar_t *vk_lmaptexturemode;
cvar_t *vk_aniso;
cvar_t *vk_mip_nearfilter;
cvar_t *vk_sampleshading;
cvar_t *vk_vsync;
cvar_t *vk_device_idx;
cvar_t *vk_fullscreen_exclusive;

cvar_t *vid_fullscreen;
cvar_t *vid_gamma;
cvar_t *vid_ref;
cvar_t *vid_refresh;
cvar_t *viewsize;
/*
=================
R_CullBox

Returns true if the box is completely outside the frustom
=================
*/
qboolean R_CullBox(vec3_t mins, vec3_t maxs)
{
    int i;

    if (r_nocull->value)
        return false;

    for (i = 0; i < 4; i++)
        if (BOX_ON_PLANE_SIDE(mins, maxs, &frustum[i]) == 2)
            return true;
    return false;
}

void R_RotateForEntity(entity_t *e, float *mvMatrix)
{
    Mat_Rotate(mvMatrix, -e->angles[2], 1.f, 0.f, 0.f);
    Mat_Rotate(mvMatrix, -e->angles[0], 0.f, 1.f, 0.f);
    Mat_Rotate(mvMatrix, e->angles[1], 0.f, 0.f, 1.f);
    Mat_Translate(mvMatrix, e->origin[0], e->origin[1], e->origin[2]);
}

/*
=============================================================

  SPRITE MODELS

=============================================================
*/

/*
=================
R_DrawSpriteModel

=================
*/
void R_DrawSpriteModel(entity_t *e)
{
    float alpha = 1.0F;
    vec3_t point;
    dsprframe_t *frame;
    float *up, *right;
    dsprite_t *psprite;

    // don't even bother culling, because it's just a single
    // polygon without a surface cache

    psprite = (dsprite_t *)currentmodel->extradata;

    e->frame %= psprite->numframes;

    frame = &psprite->frames[e->frame];

    // normal sprite
    up = vup;
    right = vright;

    if (e->flags & RF_TRANSLUCENT)
        alpha = e->alpha;

    vec3_t spriteQuad[4];

    VectorMA(e->origin, -frame->origin_y, up, point);
    VectorMA(point, -frame->origin_x, right, spriteQuad[0]);
    VectorMA(e->origin, frame->height - frame->origin_y, up, point);
    VectorMA(point, -frame->origin_x, right, spriteQuad[1]);
    VectorMA(e->origin, frame->height - frame->origin_y, up, point);
    VectorMA(point, frame->width - frame->origin_x, right, spriteQuad[2]);
    VectorMA(e->origin, -frame->origin_y, up, point);
    VectorMA(point, frame->width - frame->origin_x, right, spriteQuad[3]);

    float quadVerts[] = {
        spriteQuad[0][0], spriteQuad[0][1], spriteQuad[0][2], 0.f, 1.f,
        spriteQuad[1][0], spriteQuad[1][1], spriteQuad[1][2], 0.f, 0.f,
        spriteQuad[2][0], spriteQuad[2][1], spriteQuad[2][2], 1.f, 0.f,
        spriteQuad[0][0], spriteQuad[0][1], spriteQuad[0][2], 0.f, 1.f,
        spriteQuad[2][0], spriteQuad[2][1], spriteQuad[2][2], 1.f, 0.f,
        spriteQuad[3][0], spriteQuad[3][1], spriteQuad[3][2], 1.f, 1.f,
    };

    cmdBindPipeline(pCmd, drawSpritePipeline);
    GRA_BindUniformBuffer(pCmd, &alpha, sizeof(alpha));
    constexpr uint32_t stride = sizeof(float) * 5;
    GRA_BindVertexBuffer(pCmd, quadVerts, sizeof(quadVerts), stride);

    cmdBindPushConstants(pCmd, pRootSignature, gPushConstant, &r_viewproj_matrix);
    cmdDraw(pCmd, 6, 0);
}

//==================================================================================

/*
=============
R_DrawNullModel
=============
*/
void R_DrawNullModel(void)
{
    vec3_t shadelight;
    int i, j;

    if (currententity->flags & RF_FULLBRIGHT)
        shadelight[0] = shadelight[1] = shadelight[2] = 1.0F;
    else
        R_LightPoint(currententity->origin, shadelight);

    float model[16];
    Mat_Identity(model);
    R_RotateForEntity(currententity, model);

    vec3_t verts[24];
    verts[0][0] = 0.f;
    verts[0][1] = 0.f;
    verts[0][2] = -16.f;
    verts[1][0] = shadelight[0];
    verts[1][1] = shadelight[1];
    verts[1][2] = shadelight[2];

    for (i = 2, j = 0; i < 12; i += 2, j++)
    {
        verts[i][0] = 16 * cos(j * M_PI / 2);
        verts[i][1] = 16 * sin(j * M_PI / 2);
        verts[i][2] = 0.f;
        verts[i + 1][0] = shadelight[0];
        verts[i + 1][1] = shadelight[1];
        verts[i + 1][2] = shadelight[2];
    }

    verts[12][0] = 0.f;
    verts[12][1] = 0.f;
    verts[12][2] = 16.f;
    verts[13][0] = shadelight[0];
    verts[13][1] = shadelight[1];
    verts[13][2] = shadelight[2];

    for (i = 23, j = 4; i > 13; i -= 2, j--)
    {
        verts[i - 1][0] = 16 * cos(j * M_PI / 2);
        verts[i - 1][1] = 16 * sin(j * M_PI / 2);
        verts[i - 1][2] = 0.f;
        verts[i][0] = shadelight[0];
        verts[i][1] = shadelight[1];
        verts[i][2] = shadelight[2];
    }

    cmdBindPipeline(pCmd, drawNullModelPipeline);
    cmdBindPushConstants(pCmd, pRootSignature, gPushConstant, &r_viewproj_matrix);
    GRA_BindUniformBuffer(pCmd, &model, sizeof(model));

    constexpr uint32_t stride = sizeof(vec3_t);
    GRA_BindVertexBuffer(pCmd, verts, sizeof(verts), stride);

    auto indexCount = GRA_BindTriangleFanIBO(pCmd, 12);
    cmdDrawIndexed(pCmd, indexCount, 0, 0);
    cmdDrawIndexed(pCmd, indexCount, 0, 6);
}

/*
=============
R_DrawEntitiesOnList
=============
*/
void R_DrawEntitiesOnList(void)
{
    int i;

    if (!r_drawentities->value)
        return;

    // draw non-transparent first
    for (i = 0; i < r_newrefdef.num_entities; i++)
    {
        currententity = &r_newrefdef.entities[i];
        if (currententity->flags & RF_TRANSLUCENT)
            continue; // solid

        if (currententity->flags & RF_BEAM)
        {
            R_DrawBeam(currententity);
        }
        else
        {
            currentmodel = currententity->model;
            if (!currentmodel)
            {
                R_DrawNullModel();
                continue;
            }
            switch (currentmodel->type)
            {
            case mod_alias:
                R_DrawAliasModel(currententity);
                break;
            case mod_brush:
                R_DrawBrushModel(currententity);
                break;
            case mod_sprite:
                R_DrawSpriteModel(currententity);
                break;
            default:
                LOGF(eERROR, "Bad modeltype");
                break;
            }
        }
    }

    // draw transparent entities
    // we could sort these if it ever becomes a problem...
    for (i = 0; i < r_newrefdef.num_entities; i++)
    {
        currententity = &r_newrefdef.entities[i];
        if (!(currententity->flags & RF_TRANSLUCENT))
            continue; // solid

        if (currententity->flags & RF_BEAM)
        {
            R_DrawBeam(currententity);
        }
        else
        {
            currentmodel = currententity->model;

            if (!currentmodel)
            {
                R_DrawNullModel();
                continue;
            }
            switch (currentmodel->type)
            {
            case mod_alias:
                R_DrawAliasModel(currententity);
                break;
            case mod_brush:
                R_DrawBrushModel(currententity);
                break;
            case mod_sprite:
                R_DrawSpriteModel(currententity);
                break;
            default:
                LOGF(eERROR, "Bad modeltype");
                break;
            }
        }
    }
}

/*
** Vk_DrawParticles
**
*/
void Vk_DrawParticles(int num_particles, const particle_t particles[], const unsigned *colortable)
{
    const particle_t *p;
    int i;
    vec3_t up, right;
    float scale;
    byte color[4];

    if (!num_particles)
        return;

    VectorScale(vup, 1.5, up);
    VectorScale(vright, 1.5, right);

    typedef struct
    {
        float x, y, z, r, g, b, a, u, v;
    } pvertex;

    static pvertex visibleParticles[MAX_PARTICLES * 3];

    for (p = particles, i = 0; i < num_particles; i++, p++)
    {
        // hack a scale up to keep particles from disapearing
        scale = (p->origin[0] - r_origin[0]) * vpn[0] + (p->origin[1] - r_origin[1]) * vpn[1] +
                (p->origin[2] - r_origin[2]) * vpn[2];

        if (scale < 20)
            scale = 1;
        else
            scale = 1 + scale * 0.004;

        *(int *)color = colortable[p->color];

        int idx = i * 3;
        float r = color[0] / 255.f;
        float g = color[1] / 255.f;
        float b = color[2] / 255.f;

        visibleParticles[idx].x = p->origin[0];
        visibleParticles[idx].y = p->origin[1];
        visibleParticles[idx].z = p->origin[2];
        visibleParticles[idx].r = r;
        visibleParticles[idx].g = g;
        visibleParticles[idx].b = b;
        visibleParticles[idx].a = p->alpha;
        visibleParticles[idx].u = 0.0625;
        visibleParticles[idx].v = 0.0625;

        visibleParticles[idx + 1].x = p->origin[0] + up[0] * scale;
        visibleParticles[idx + 1].y = p->origin[1] + up[1] * scale;
        visibleParticles[idx + 1].z = p->origin[2] + up[2] * scale;
        visibleParticles[idx + 1].r = r;
        visibleParticles[idx + 1].g = g;
        visibleParticles[idx + 1].b = b;
        visibleParticles[idx + 1].a = p->alpha;
        visibleParticles[idx + 1].u = 1.0625;
        visibleParticles[idx + 1].v = 0.0625;

        visibleParticles[idx + 2].x = p->origin[0] + right[0] * scale;
        visibleParticles[idx + 2].y = p->origin[1] + right[1] * scale;
        visibleParticles[idx + 2].z = p->origin[2] + right[2] * scale;
        visibleParticles[idx + 2].r = r;
        visibleParticles[idx + 2].g = g;
        visibleParticles[idx + 2].b = b;
        visibleParticles[idx + 2].a = p->alpha;
        visibleParticles[idx + 2].u = 0.0625;
        visibleParticles[idx + 2].v = 1.0625;
    }

    cmdBindPipeline(pCmd, drawParticlesPipeline);
    uint32_t vaoSize = 3 * sizeof(pvertex) * num_particles;

    constexpr uint32_t stride = sizeof(pvertex);
    GRA_BindVertexBuffer(pCmd, &visibleParticles, vaoSize, stride);
    cmdBindPushConstants(pCmd, pRootSignature, gPushConstant, &r_viewproj_matrix);
    cmdBindDescriptorSet(pCmd, 0, pDescriptorSetsTexture[r_particletexture->index]);
    cmdDraw(pCmd, 3 * num_particles, 0);
}

/*
===============
R_DrawParticles
===============
*/
void R_DrawParticles(void)
{
    if (vk_point_particles->value)
    {
        int i;
        unsigned char color[4];
        const particle_t *p;

        if (!r_newrefdef.num_particles)
            return;

        typedef struct
        {
            float x, y, z, r, g, b, a;
        } ppoint;

        struct
        {
            float particleSize;
            float particleScale;
            float minPointSize;
            float maxPointSize;
            float att_a;
            float att_b;
            float att_c;
        } particleUbo;

        particleUbo.particleSize = vk_particle_size->value;
        particleUbo.particleScale = vid.width * Cvar_Get("viewsize", "100", CVAR_ARCHIVE)->value / 102400;
        particleUbo.minPointSize = vk_particle_min_size->value;
        particleUbo.maxPointSize = vk_particle_max_size->value;
        particleUbo.att_a = vk_particle_att_a->value;
        particleUbo.att_b = vk_particle_att_b->value;
        particleUbo.att_c = vk_particle_att_c->value;

        static ppoint visibleParticles[MAX_PARTICLES];

        for (i = 0, p = r_newrefdef.particles; i < r_newrefdef.num_particles; i++, p++)
        {
            *(int *)color = d_8to24table[p->color];

            float r = color[0] / 255.f;
            float g = color[1] / 255.f;
            float b = color[2] / 255.f;

            visibleParticles[i].x = p->origin[0];
            visibleParticles[i].y = p->origin[1];
            visibleParticles[i].z = p->origin[2];
            visibleParticles[i].r = r;
            visibleParticles[i].g = g;
            visibleParticles[i].b = b;
            visibleParticles[i].a = p->alpha;
        }

        cmdBindPipeline(pCmd, drawPointParticlesPipeline);
        GRA_BindUniformBuffer(pCmd, &particleUbo, sizeof(particleUbo));

        constexpr uint32_t stride = sizeof(ppoint);
        GRA_BindVertexBuffer(pCmd, visibleParticles, sizeof(ppoint) * r_newrefdef.num_particles, stride);
        cmdBindPushConstants(pCmd, pRootSignature, gPushConstant, &r_viewproj_matrix);

        cmdDraw(pCmd, r_newrefdef.num_particles, 0);
    }
    else
    {
        Vk_DrawParticles(r_newrefdef.num_particles, r_newrefdef.particles, d_8to24table);
    }
}

/*
============
R_PolyBlend
============
*/
void R_PolyBlend(void)
{
    if (!vk_polyblend->value)
        return;
    if (!v_blend[3])
        return;

    float polyTransform[] = {0.f, 0.f, vid.width, vid.height, v_blend[0], v_blend[1], v_blend[2], v_blend[3]};
    GRA_DrawColorRect(polyTransform, sizeof(polyTransform), RenderPass::WORLD);
}

//=======================================================================

int SignbitsForPlane(cplane_t *out)
{
    int bits, j;

    // for fast box on planeside test

    bits = 0;
    for (j = 0; j < 3; j++)
    {
        if (out->normal[j] < 0)
            bits |= 1 << j;
    }
    return bits;
}

void R_SetFrustum(float fovx, float fovy)
{
    // rotate VPN right by FOV_X/2 degrees
    RotatePointAroundVector(frustum[0].normal, vup, vpn, -(90 - fovx / 2));
    // rotate VPN left by FOV_X/2 degrees
    RotatePointAroundVector(frustum[1].normal, vup, vpn, 90 - fovx / 2);
    // rotate VPN up by FOV_X/2 degrees
    RotatePointAroundVector(frustum[2].normal, vright, vpn, 90 - fovy / 2);
    // rotate VPN down by FOV_X/2 degrees
    RotatePointAroundVector(frustum[3].normal, vright, vpn, -(90 - fovy / 2));

    for (int i = 0; i < 4; i++)
    {
        frustum[i].type = PLANE_ANYZ;
        frustum[i].dist = DotProduct(r_origin, frustum[i].normal);
        frustum[i].signbits = SignbitsForPlane(&frustum[i]);
    }
}

//=======================================================================

/*
===============
R_SetupFrame
===============
*/
void R_SetupFrame(void)
{
    int i;
    mleaf_t *leaf;

    r_framecount++;

    // build the transformation matrix for the given view angles
    VectorCopy(r_newrefdef.vieworg, r_origin);

    AngleVectors(r_newrefdef.viewangles, vpn, vright, vup);

    // current viewcluster
    if (!(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
    {
        r_oldviewcluster = r_viewcluster;
        r_oldviewcluster2 = r_viewcluster2;
        leaf = Mod_PointInLeaf(r_origin, r_worldmodel);
        r_viewcluster = r_viewcluster2 = leaf->cluster;

        // check above and below so crossing solid water doesn't draw wrong
        if (!leaf->contents)
        { // look down a bit
            vec3_t temp;

            VectorCopy(r_origin, temp);
            temp[2] -= 16;
            leaf = Mod_PointInLeaf(temp, r_worldmodel);
            if (!(leaf->contents & CONTENTS_SOLID) && (leaf->cluster != r_viewcluster2))
                r_viewcluster2 = leaf->cluster;
        }
        else
        { // look up a bit
            vec3_t temp;

            VectorCopy(r_origin, temp);
            temp[2] += 16;
            leaf = Mod_PointInLeaf(temp, r_worldmodel);
            if (!(leaf->contents & CONTENTS_SOLID) && (leaf->cluster != r_viewcluster2))
                r_viewcluster2 = leaf->cluster;
        }
    }

    for (i = 0; i < 4; i++)
        v_blend[i] = r_newrefdef.blend[i];

    c_brush_polys = 0;
    c_alias_polys = 0;

    // clear out the portion of the screen that the NOWORLDMODEL defines
    // unlike OpenGL, draw a rectangle in proper location - it's easier to do in Vulkan
    if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
    {
        float clearArea[] = {(float)r_newrefdef.x / vid.width,
                             (float)r_newrefdef.y / vid.height,
                             (float)r_newrefdef.width / vid.width,
                             (float)r_newrefdef.height / vid.height,
                             .3f,
                             .3f,
                             .3f,
                             1.f};
        GRA_DrawColorRect(clearArea, sizeof(clearArea), RenderPass::UI);
    }
}

/*
=============
R_SetupVulkan
=============
*/
void R_SetupVulkan(void)
{
    int x, x2, y2, y, w, h;

    //
    // set up viewport
    //
    x = floor(r_newrefdef.x * vid.width / vid.width);
    x2 = ceil((r_newrefdef.x + r_newrefdef.width) * vid.width / vid.width);
    y = floor(vid.height - r_newrefdef.y * vid.height / vid.height);
    y2 = ceil(vid.height - (r_newrefdef.y + r_newrefdef.height) * vid.height / vid.height);

    w = x2 - x;
    h = y - y2;

    cmdSetViewport(pCmd, x, vid.height - h - y2, w, h, 0, 1);

    // set up projection matrix
    r_proj_fovx = r_newrefdef.fov_x;
    r_proj_fovy = r_newrefdef.fov_y;
    r_proj_aspect = (float)r_newrefdef.width / r_newrefdef.height;

    //Mat_Perspective(toFloatPtr(r_projection_matrix), toFloatPtr(r_vulkan_correction), r_proj_fovy, r_proj_aspect, 4, 4096);
    r_projection_matrix = mat4::perspectiveRH(degToRad(r_proj_fovx), 1/r_proj_aspect, 4, 4096);
    //r_projection_matrix = r_vulkan_correction * r_projection_matrix;

    R_SetFrustum(r_proj_fovx, r_proj_fovy);

    // set up view matrix
    r_view_matrix = mat4::identity();
    // put Z going up
    r_view_matrix =
        mat4::translation({-r_newrefdef.vieworg[0], -r_newrefdef.vieworg[1], -r_newrefdef.vieworg[2]}) * r_view_matrix;

    r_view_matrix = mat4::rotation(degToRad(-r_newrefdef.viewangles[1]), {0.f, 0.f, 1.f}) * r_view_matrix;
    r_view_matrix = mat4::rotation(degToRad(-r_newrefdef.viewangles[0]), {0.f, 1.f, 0.f}) * r_view_matrix;
    r_view_matrix = mat4::rotation(degToRad(-r_newrefdef.viewangles[2]), {1.f, 0.f, 0.f}) * r_view_matrix;

    r_view_matrix = mat4::rotation(degToRad(90), {0.f, 0.f, 1.f}) * r_view_matrix;
    r_view_matrix = mat4::rotation(degToRad(-90), {1.f, 0.f, 0.f}) * r_view_matrix;

    // precalculate view-projection matrix
    r_viewproj_matrix = r_projection_matrix * r_view_matrix;
    // Mat_Mul(toFloatPtr(r_view_matrix), r_projection_matrix, toFloatPtr(r_viewproj_matrix));
}

void R_Flash(void)
{
    R_PolyBlend();
}

/*
================
R_RenderView

r_newrefdef must be set before the first call
================
*/
void R_RenderView(refdef_t *fd)
{
    if (r_norefresh->value)
        return;

    r_newrefdef = *fd;

    if (!r_worldmodel && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
        LOGF(eERROR, "R_RenderView: NULL worldmodel");

    if (r_speeds->value)
    {
        c_brush_polys = 0;
        c_alias_polys = 0;
    }

    cmdSetScissor(pCmd, r_newrefdef.x, r_newrefdef.y, r_newrefdef.width, r_newrefdef.height);

    R_PushDlights();

    // added for compatibility sake with OpenGL implementation - don't use it!

    R_SetupFrame();

    R_SetupVulkan();

    R_MarkLeaves(); // done here so we know if we're in water

    R_DrawWorld();

    R_DrawEntitiesOnList();

    R_RenderDlights();

    R_DrawParticles();

    R_DrawAlphaSurfaces();

    R_Flash();

    if (r_speeds->value)
    {
        LOGF(eINFO, "%4i wpoly %4i epoly %i tex %i lmaps\n", c_brush_polys, c_alias_polys, c_visible_textures,
             c_visible_lightmaps);
    }
}

void R_EndWorldRenderpass(void)
{
    if (!vk_frameStarted)
        return;

    cmdEndGpuTimestampQuery(pCmd, gGpuProfileToken);
    RenderTargetBarrier barriers[2]{
        {
            pWorldRenderTarget,
            RESOURCE_STATE_RENDER_TARGET,
            RESOURCE_STATE_PRESENT,
        },
        {
            pWorldWarpRenderTarget,
            RESOURCE_STATE_SHADER_RESOURCE,
            RESOURCE_STATE_RENDER_TARGET,

        },
    };
    cmdResourceBarrier(pCmd, 0, NULL, 0, NULL, 2, barriers);

    BindRenderTargetsDesc bindRenderTargets = {};
    bindRenderTargets.mRenderTargetCount = 1;
    bindRenderTargets.mRenderTargets[0] = {pWorldWarpRenderTarget, LOAD_ACTION_CLEAR};
    bindRenderTargets.mDepthStencil = {pDepthBuffer, LOAD_ACTION_CLEAR};
    cmdBindRenderTargets(pCmd, &bindRenderTargets);

    cmdSetViewport(pCmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
    cmdSetScissor(pCmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);

    float pushConsts[] = {(r_newrefdef.rdflags & RDF_UNDERWATER) && vk_underwater->value > 0 ? r_newrefdef.time : 0.f,
                          viewsize->value / 100, (float)vid.width, (float)vid.height};

    cmdBeginGpuTimestampQuery(pCmd, gGpuProfileToken, "Game World Water Effect");

    cmdBindPushConstants(pCmd, pRootSignature, gPushConstant, pushConsts);
    cmdBindDescriptorSet(pCmd, 0, pDescriptorSetWorldTexture);
    cmdBindPipeline(pCmd, worldWarpPipeline);
    cmdDraw(pCmd, 3, 0);

    cmdEndGpuTimestampQuery(pCmd, gGpuProfileToken);

    barriers[0] = {
        pWorldWarpRenderTarget,
        RESOURCE_STATE_RENDER_TARGET,
        RESOURCE_STATE_SHADER_RESOURCE,
    };
    barriers[1] = {
        pRenderTarget,
        RESOURCE_STATE_PRESENT,
        RESOURCE_STATE_RENDER_TARGET,
    };

    cmdResourceBarrier(pCmd, 0, NULL, 0, NULL, 2, barriers);

    bindRenderTargets = {};
    bindRenderTargets.mRenderTargetCount = 1;
    bindRenderTargets.mRenderTargets[0] = {pRenderTarget, LOAD_ACTION_CLEAR};
    bindRenderTargets.mDepthStencil = {pDepthBuffer, LOAD_ACTION_CLEAR};
    cmdBindRenderTargets(pCmd, &bindRenderTargets);

    cmdBeginGpuTimestampQuery(pCmd, gGpuProfileToken, "Game UI Pass");
    // // start drawing UI
    // QVk_BeginRenderpass(RP_UI);
}

void R_SetVulkan2D(void)
{
    // player configuration screen renders a model using the UI renderpass, so skip finishing RP_WORLD twice
    if (!(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
        R_EndWorldRenderpass();

    cmdSetViewport(pCmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
    cmdSetScissor(pCmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);

    if (!(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
    {
        float pushConsts[] = {vk_postprocess->value, vid_gamma->value};
        cmdBindPushConstants(pCmd, pRootSignature, gPushConstant, pushConsts);
        cmdBindDescriptorSet(pCmd, 0, pDescriptorSetWorldWarpTexture);
        cmdBindPipeline(pCmd, postprocessPipeline);
        cmdDraw(pCmd, 3, 0);
    }
}

/*
====================
R_SetLightLevel

====================
*/
void R_SetLightLevel(void)
{
    vec3_t shadelight;

    if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
        return;

    // save off light value for server to look at (BIG HACK!)

    R_LightPoint(r_newrefdef.vieworg, shadelight);

    // pick the greatest component, which should be the same
    // as the mono value returned by software
    if (shadelight[0] > shadelight[1])
    {
        if (shadelight[0] > shadelight[2])
            r_lightlevel->value = 150 * shadelight[0];
        else
            r_lightlevel->value = 150 * shadelight[2];
    }
    else
    {
        if (shadelight[1] > shadelight[2])
            r_lightlevel->value = 150 * shadelight[1];
        else
            r_lightlevel->value = 150 * shadelight[2];
    }
}

/*
@@@@@@@@@@@@@@@@@@@@@
R_RenderFrame

@@@@@@@@@@@@@@@@@@@@@
*/
void R_RenderFrame(refdef_t *fd)
{
    if (!vk_frameStarted)
        return;

    R_RenderView(fd);
    R_SetLightLevel();
    R_SetVulkan2D();
}

void R_Register(void)
{
    r_lefthand = Cvar_Get("hand", "0", CVAR_USERINFO | CVAR_ARCHIVE);
    r_norefresh = Cvar_Get("r_norefresh", "0", 0);
    r_fullbright = Cvar_Get("r_fullbright", "0", 0);
    r_drawentities = Cvar_Get("r_drawentities", "1", 0);
    r_drawworld = Cvar_Get("r_drawworld", "1", 0);
    r_novis = Cvar_Get("r_novis", "0", 0);
    r_nocull = Cvar_Get("r_nocull", "0", 0);
    r_lerpmodels = Cvar_Get("r_lerpmodels", "1", 0);
    r_speeds = Cvar_Get("r_speeds", "0", 0);
    r_lightlevel = Cvar_Get("r_lightlevel", "0", 0);
#if defined(_DEBUG)
    vk_validation = Cvar_Get("vk_validation", "2", 0);
#else
    vk_validation = Cvar_Get("vk_validation", "0", 0);
#endif
    vk_mode = Cvar_Get("vk_mode", "11", CVAR_ARCHIVE);
    vk_log = Cvar_Get("vk_log", "0", 0);
    vk_picmip = Cvar_Get("vk_picmip", "0", 0);
    vk_skymip = Cvar_Get("vk_skymip", "0", 0);
    vk_round_down = Cvar_Get("vk_round_down", "1", 0);
    vk_flashblend = Cvar_Get("vk_flashblend", "0", 0);
    vk_finish = Cvar_Get("vk_finish", "0", CVAR_ARCHIVE);
    vk_clear = Cvar_Get("vk_clear", "0", CVAR_ARCHIVE);
    vk_lockpvs = Cvar_Get("vk_lockpvs", "0", 0);
    vk_polyblend = Cvar_Get("vk_polyblend", "1", 0);
    vk_modulate = Cvar_Get("vk_modulate", "1", CVAR_ARCHIVE);
    vk_shadows = Cvar_Get("vk_shadows", "0", CVAR_ARCHIVE);
    vk_particle_size = Cvar_Get("vk_particle_size", "40", CVAR_ARCHIVE);
    vk_particle_att_a = Cvar_Get("vk_particle_att_a", "0.01", CVAR_ARCHIVE);
    vk_particle_att_b = Cvar_Get("vk_particle_att_b", "0.0", CVAR_ARCHIVE);
    vk_particle_att_c = Cvar_Get("vk_particle_att_c", "0.01", CVAR_ARCHIVE);
    vk_particle_min_size = Cvar_Get("vk_particle_min_size", "2", CVAR_ARCHIVE);
    vk_particle_max_size = Cvar_Get("vk_particle_max_size", "40", CVAR_ARCHIVE);
    vk_point_particles = Cvar_Get("vk_point_particles", "1", CVAR_ARCHIVE);
    vk_postprocess = Cvar_Get("vk_postprocess", "1", CVAR_ARCHIVE);
    vk_underwater = Cvar_Get("vk_underwater", "1", CVAR_ARCHIVE);
    vk_dynamic = Cvar_Get("vk_dynamic", "1", 0);
    vk_msaa = Cvar_Get("vk_msaa", "0", CVAR_ARCHIVE);
    vk_showtris = Cvar_Get("vk_showtris", "0", 0);
    vk_lightmap = Cvar_Get("vk_lightmap", "0", 0);
    vk_texturemode = Cvar_Get("vk_texturemode", "VK_MIPMAP_LINEAR", CVAR_ARCHIVE);
    vk_lmaptexturemode = Cvar_Get("vk_lmaptexturemode", "VK_MIPMAP_LINEAR", CVAR_ARCHIVE);
    vk_aniso = Cvar_Get("vk_aniso", "1", CVAR_ARCHIVE);
    vk_mip_nearfilter = Cvar_Get("vk_mip_nearfilter", "0", CVAR_ARCHIVE);
    vk_sampleshading = Cvar_Get("vk_sampleshading", "0", CVAR_ARCHIVE);
    vk_vsync = Cvar_Get("vk_vsync", "0", CVAR_ARCHIVE);
    vk_device_idx = Cvar_Get("vk_device", "-1", CVAR_ARCHIVE);
    vk_fullscreen_exclusive = Cvar_Get("vk_fullscreen_exclusive", "1", CVAR_ARCHIVE);
    // clamp vk_msaa to accepted range so that video menu doesn't crash on us
    if (vk_msaa->value < 0)
        Cvar_Set("vk_msaa", "0");
    else if (vk_msaa->value > 4)
        Cvar_Set("vk_msaa", "4");

    vid_fullscreen = Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);
    vid_gamma = Cvar_Get("vid_gamma", "1.0", CVAR_ARCHIVE);
    vid_ref = Cvar_Get("vid_ref", "soft", CVAR_ARCHIVE);
    vid_refresh = Cvar_Get("vid_refresh", "0", CVAR_NOSET);
    viewsize = Cvar_Get("viewsize", "100", CVAR_ARCHIVE);

    // Cmd_AddCommand("vk_strings", Vk_Strings_f);
    // Cmd_AddCommand("vk_restart", Vk_PollRestart_f);
    // Cmd_AddCommand("vk_mem", Vk_Mem_f);
    // Cmd_AddCommand("imagelist", GRA_ImageList_f);
    // Cmd_AddCommand("screenshot", Vk_ScreenShot_f);
}

/*
==================
R_SetMode
==================
*/
qboolean R_SetMode(void)
{
    // rserr_t err;
    // qboolean fullscreen;

    // /// TODO: temporary disable fullscreen to allow debugging.
    // fullscreen = false; //vid_fullscreen->value;

    // vid_gamma->modified = false;
    // vid_fullscreen->modified = false;
    // vk_mode->modified = false;
    // vk_msaa->modified = false;
    // vk_clear->modified = false;
    // vk_validation->modified = false;
    // vk_mip_nearfilter->modified = false;
    // vk_sampleshading->modified = false;
    // vk_vsync->modified = false;
    // vk_modulate->modified = false;
    // vk_device_idx->modified = false;
    // vk_fullscreen_exclusive->modified = false;
    // vk_picmip->modified = false;
    // // refresh texture samplers
    // vk_texturemode->modified = true;
    // vk_lmaptexturemode->modified = true;

    // if ((err = Vkimp_SetMode((int*)&vid.width, (int*)&vid.height, vk_mode->value, fullscreen)) == rserr_ok)
    // {
    // 	vk_state.prev_mode = vk_mode->value;
    // }
    // else
    // {
    // 	if (err == rserr_invalid_fullscreen)
    // 	{
    // 		Cvar_SetValue("vid_fullscreen", 0);
    // 		vid_fullscreen->modified = false;
    // 		LOGF(eINFO, "ref_vk::R_SetMode() - fullscreen unavailable in this mode\n");
    // 		if ((err = Vkimp_SetMode((int*)&vid.width, (int*)&vid.height, vk_mode->value, false)) == rserr_ok)
    // 			return true;
    // 	}
    // 	else if (err == rserr_invalid_mode)
    // 	{
    // 		Cvar_SetValue("vk_mode", vk_state.prev_mode);
    // 		vk_mode->modified = false;
    // 		LOGF(eINFO, "ref_vk::R_SetMode() - invalid mode\n");
    // 	}

    // 	// try setting it back to something safe
    // 	if ((err = Vkimp_SetMode((int*)&vid.width, (int*)&vid.height, vk_state.prev_mode, false)) != rserr_ok)
    // 	{
    // 		LOGF(eINFO, "ref_vk::R_SetMode() - could not revert to safe mode\n");
    // 		return false;
    // 	}
    // }
    return true;
}

/*
===============
R_Init
===============
*/
qboolean R_Init(/* void *hinstance, void *hWnd */)
{
    // LOGF(eINFO, "ref_vk version: "REF_VERSION"\n");

    R_Register();

    // // create the window (OS-specific)
    // if (!Vkimp_Init(hinstance, hWnd))
    // {
    // 	return false;
    // }

    // // set our "safe" modes
    // vk_state.prev_mode = 6;
    // // set video mode/screen resolution
    // if (!R_SetMode())
    // {
    // 	LOGF(eINFO, "ref_vk::R_Init() - could not R_SetMode()\n");
    // 	return false;
    // }
    // Vid_MenuInit();

    // // window is ready, initialize Vulkan now
    // if (!QVk_Init())
    // {
    // 	LOGF(eINFO, "ref_vk::R_Init() - could not initialize Vulkan!\n");
    // 	return false;
    // }

    // LOGF(eINFO, "Successfully initialized Vulkan!\n");
    // // print device information during startup
    // Vk_Strings_f();

    GRA_InitImages();
    Mod_Init();
    R_InitParticleTexture();
    Draw_InitLocal();

    return true;
}

/*
===============
R_Shutdown
===============
*/
void R_Shutdown(void)
{
    // Cmd_RemoveCommand("vk_strings");
    // Cmd_RemoveCommand("vk_mem");
    // Cmd_RemoveCommand("vk_restart");
    // Cmd_RemoveCommand("imagelist");
    // Cmd_RemoveCommand("screenshot");

    // vkDeviceWaitIdle(vk_device.logical);

    Mod_FreeAll();
    GRA_ShutdownImages();

    // // Shutdown Vulkan subsystem
    // QVk_Shutdown();
    // // shut down OS specific Vulkan stuff (in our case: window)
    // Vkimp_Shutdown();
}

static GpuCmdRingElement elem;
static uint32_t swapchainImageIndex;
/*
@@@@@@@@@@@@@@@@@@@@@
R_BeginFrame
@@@@@@@@@@@@@@@@@@@@@
*/
void R_BeginFrame(float camera_separation)
{
    if (pSwapChain->mEnableVsync != pApp->mSettings.mVSyncEnabled)
    {
        waitQueueIdle(pGraphicsQueue);
        ::toggleVSync(pRenderer, &pSwapChain);
    }

    acquireNextImage(pRenderer, pSwapChain, pImageAcquiredSemaphore, NULL, &swapchainImageIndex);

    pRenderTarget = pSwapChain->ppRenderTargets[swapchainImageIndex];
    elem = getNextGpuCmdRingElement(&gGraphicsCmdRing, true, 1);

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
    cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "Game World Pass");
    pCmd = cmd;

    RenderTargetBarrier barriers[] = {
        {
            pWorldRenderTarget,
            RESOURCE_STATE_SHADER_RESOURCE,
            RESOURCE_STATE_RENDER_TARGET,
        },
    };
    cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 1, barriers);

    // simply record the screen cleaning command
    BindRenderTargetsDesc bindRenderTargets = {};
    bindRenderTargets.mRenderTargetCount = 1;
    bindRenderTargets.mRenderTargets[0] = {pWorldRenderTarget, LOAD_ACTION_CLEAR};
    bindRenderTargets.mDepthStencil = {pDepthBuffer, LOAD_ACTION_CLEAR};
    cmdBindRenderTargets(cmd, &bindRenderTargets);
    cmdSetViewport(cmd, 0.0f, 0.0f, (float)pWorldRenderTarget->mWidth, (float)pWorldRenderTarget->mHeight, 0.0f, 1.0f);
    cmdSetScissor(cmd, 0, 0, pWorldRenderTarget->mWidth, pWorldRenderTarget->mHeight);

    // // if Sys_Error() had been issued mid-frame, we might end up here without properly submitting the image, so
    // call QVk_EndFrame to be safe if (QVk_EndFrame(true) != VK_SUCCESS)
    // {
    // 	Vk_PollRestart_f();
    // 	return;
    // }
    // /*
    // ** change modes if necessary
    // */
    // if (vk_mode->modified || vid_fullscreen->modified || vk_texturemode->modified ||
    // 	vk_lmaptexturemode->modified || vk_aniso->modified || vk_device_idx->modified)
    // {
    // 	if (vk_texturemode->modified || vk_lmaptexturemode->modified || vk_aniso->modified)
    // 	{
    // 		if (vk_texturemode->modified || vk_aniso->modified)
    // 		{
    // 			Vk_TextureMode(vk_texturemode->string);
    // 			vk_texturemode->modified = false;
    // 		}
    // 		if (vk_lmaptexturemode->modified || vk_aniso->modified)
    // 		{
    // 			Vk_LmapTextureMode(vk_lmaptexturemode->string);
    // 			vk_lmaptexturemode->modified = false;
    // 		}

    // 		vk_aniso->modified = false;
    // 	}
    // 	else
    // 	{
    // 		cvar_t	*ref = Cvar_Get("vid_ref", "vk", 0);
    // 		ref->modified = true;
    // 	}
    // }

    // if (vk_log->modified)
    // {
    // 	Vkimp_EnableLogging(vk_log->value);
    // 	vk_log->modified = false;
    // }

    // if (vk_log->value)
    // {
    // 	Vkimp_LogNewFrame();
    // }

    // Vkimp_BeginFrame(camera_separation);

    // VkResult swapChainValid = QVk_BeginFrame();
    // // if the swapchain is invalid, just recreate the video system and revert to safe windowed mode
    // if (swapChainValid != VK_SUCCESS)
    // {
    // 	Vk_PollRestart_f();
    // }
    // else
    // {
    // 	QVk_BeginRenderpass(RP_WORLD);
    // }

    gFrameIndex = (gFrameIndex + 1) % gDataBufferCount;

    resetGPURingBuffer(&dynamicIndexBuffers[gFrameIndex]);
    resetGPURingBuffer(&dynamicUniformBuffers[gFrameIndex]);
    resetGPURingBuffer(&dynamicVertexBuffers[gFrameIndex]);

    vk_frameStarted = true;
}

static qboolean R_ShouldRestart()
{
    // 	qboolean fs_exclusive_modified = vid_fullscreen->value && vk_fullscreen_exclusive->modified;
    // 	return	vk_restart || vk_validation->modified || vk_msaa->modified || vk_clear->modified ||
    // 			vk_picmip->modified || vid_gamma->modified || vk_mip_nearfilter->modified ||
    // 			vk_sampleshading->modified || vk_vsync->modified || vk_modulate->modified
    // #ifdef FULL_SCREEN_EXCLUSIVE_ENABLED
    // 			|| fs_exclusive_modified
    // #endif
    // 			;
}

/*
@@@@@@@@@@@@@@@@@@@@@
R_EndFrame
@@@@@@@@@@@@@@@@@@@@@
*/
void R_EndFrame(void)
{
    if (!vk_frameStarted)
        return;

    cmdEndGpuTimestampQuery(pCmd, gGpuProfileToken);

    cmdBeginGpuTimestampQuery(pCmd, gGpuProfileToken, "Draw UI");
    uint32_t gFontID = 0;
    FontDrawDesc gFrameTimeDraw = {};
    gFrameTimeDraw.mFontColor = 0xff00ffff;
    gFrameTimeDraw.mFontSize = 18.0f;
    gFrameTimeDraw.mFontID = gFontID;
    float2 txtSizePx = cmdDrawCpuProfile(pCmd, float2(8.f, 15.f), &gFrameTimeDraw);
    cmdDrawGpuProfile(pCmd, float2(8.f, txtSizePx.y + 75.f), gGpuProfileToken, &gFrameTimeDraw);

    cmdDrawUserInterface(pCmd);

    cmdBindRenderTargets(pCmd, NULL);
    cmdEndGpuTimestampQuery(pCmd, gGpuProfileToken);

    RenderTargetBarrier barriers[1]{{
        pRenderTarget,
        RESOURCE_STATE_RENDER_TARGET,
        RESOURCE_STATE_PRESENT,
    }};
    cmdResourceBarrier(pCmd, 0, NULL, 0, NULL, 1, barriers);

    cmdEndGpuFrameProfile(pCmd, gGpuProfileToken);

    endCmd(pCmd);

    FlushResourceUpdateDesc flushUpdateDesc = {};
    flushUpdateDesc.mNodeIndex = 0;
    flushResourceUpdates(&flushUpdateDesc);
    Semaphore *waitSemaphores[2] = {flushUpdateDesc.pOutSubmittedSemaphore, pImageAcquiredSemaphore};

    QueueSubmitDesc submitDesc = {};
    submitDesc.mCmdCount = 1;
    submitDesc.mSignalSemaphoreCount = 1;
    submitDesc.mWaitSemaphoreCount = TF_ARRAY_COUNT(waitSemaphores);
    submitDesc.ppCmds = &pCmd;
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

    // if (QVk_EndFrame(false) != VK_SUCCESS)
    // 	Vk_PollRestart_f();

    // // restart Vulkan renderer without rebuilding the entire window
    // if (R_ShouldRestart())
    // {
    // 	vk_restart = false;
    // 	vk_validation->modified = false;
    // 	vk_msaa->modified = false;
    // 	vk_clear->modified = false;
    // 	vk_picmip->modified = false;
    // 	vid_gamma->modified = false;
    // 	vk_mip_nearfilter->modified = false;
    // 	vk_sampleshading->modified = false;
    // 	vk_vsync->modified = false;
    // 	vk_modulate->modified = false;
    // 	vk_fullscreen_exclusive->modified = false;

    // 	// shutdown
    // 	vkDeviceWaitIdle(vk_device.logical);
    // 	Mod_FreeAll();
    // 	Vk_ShutdownImages();
    // 	QVk_Shutdown();
    // 	numvktextures = 0;

    // 	// initialize
    // 	if (!QVk_Init())
    // 	{
    // 		Sys_Error(ERR_FATAL, "R_EndFrame(): could not re-initialize Vulkan!");
    // 	}

    // 	LOGF(eINFO, "Successfully restarted Vulkan!\n");

    // 	Vk_Strings_f();

    // 	Vk_InitImages();
    // 	Mod_Init();
    // 	R_InitParticleTexture();
    // 	Draw_InitLocal();

    // 	extern cvar_t *vid_refresh;
    // 	vid_refresh->modified = true;
    // }

    vk_frameStarted = false;
}

/*
=============
R_SetPalette
=============
*/
unsigned r_rawpalette[256];

void R_SetPalette(const unsigned char *palette)
{
    int i;

    byte *rp = (byte *)r_rawpalette;

    if (palette)
    {
        for (i = 0; i < 256; i++)
        {
            rp[i * 4 + 0] = palette[i * 3 + 0];
            rp[i * 4 + 1] = palette[i * 3 + 1];
            rp[i * 4 + 2] = palette[i * 3 + 2];
            rp[i * 4 + 3] = 0xff;
        }
    }
    else
    {
        for (i = 0; i < 256; i++)
        {
            rp[i * 4 + 0] = d_8to24table[i] & 0xff;
            rp[i * 4 + 1] = (d_8to24table[i] >> 8) & 0xff;
            rp[i * 4 + 2] = (d_8to24table[i] >> 16) & 0xff;
            rp[i * 4 + 3] = 0xff;
        }
    }
}

/*
** R_DrawBeam
*/
void R_DrawBeam(entity_t *e)
{
#define NUM_BEAM_SEGS 6

    int i;
    float r, g, b;

    vec3_t perpvec;
    vec3_t direction, normalized_direction;
    vec3_t start_points[NUM_BEAM_SEGS], end_points[NUM_BEAM_SEGS];
    vec3_t oldorigin, origin;

    oldorigin[0] = e->oldorigin[0];
    oldorigin[1] = e->oldorigin[1];
    oldorigin[2] = e->oldorigin[2];

    origin[0] = e->origin[0];
    origin[1] = e->origin[1];
    origin[2] = e->origin[2];

    normalized_direction[0] = direction[0] = oldorigin[0] - origin[0];
    normalized_direction[1] = direction[1] = oldorigin[1] - origin[1];
    normalized_direction[2] = direction[2] = oldorigin[2] - origin[2];

    if (VectorNormalize(normalized_direction) == 0)
        return;

    PerpendicularVector(perpvec, normalized_direction);
    VectorScale(perpvec, e->frame / 2, perpvec);

    for (i = 0; i < 6; i++)
    {
        RotatePointAroundVector(start_points[i], normalized_direction, perpvec, (360.0 / NUM_BEAM_SEGS) * i);
        VectorAdd(start_points[i], origin, start_points[i]);
        VectorAdd(start_points[i], direction, end_points[i]);
    }

    r = (d_8to24table[e->skinnum & 0xFF]) & 0xFF;
    g = (d_8to24table[e->skinnum & 0xFF] >> 8) & 0xFF;
    b = (d_8to24table[e->skinnum & 0xFF] >> 16) & 0xFF;

    r *= 1 / 255.0F;
    g *= 1 / 255.0F;
    b *= 1 / 255.0F;

    float color[4] = {r, g, b, e->alpha};

    struct
    {
        float v[3];
    } beamvertex[NUM_BEAM_SEGS * 4];

    for (i = 0; i < NUM_BEAM_SEGS; i++)
    {
        int idx = i * 4;
        beamvertex[idx].v[0] = start_points[i][0];
        beamvertex[idx].v[1] = start_points[i][1];
        beamvertex[idx].v[2] = start_points[i][2];

        beamvertex[idx + 1].v[0] = end_points[i][0];
        beamvertex[idx + 1].v[1] = end_points[i][1];
        beamvertex[idx + 1].v[2] = end_points[i][2];

        beamvertex[idx + 2].v[0] = start_points[(i + 1) % NUM_BEAM_SEGS][0];
        beamvertex[idx + 2].v[1] = start_points[(i + 1) % NUM_BEAM_SEGS][1];
        beamvertex[idx + 2].v[2] = start_points[(i + 1) % NUM_BEAM_SEGS][2];

        beamvertex[idx + 3].v[0] = end_points[(i + 1) % NUM_BEAM_SEGS][0];
        beamvertex[idx + 3].v[1] = end_points[(i + 1) % NUM_BEAM_SEGS][1];
        beamvertex[idx + 3].v[2] = end_points[(i + 1) % NUM_BEAM_SEGS][2];
    }

    cmdBindPipeline(pCmd, drawBeamPipeline);
    cmdBindPushConstants(pCmd, pRootSignature, gPushConstant, &r_viewproj_matrix);
    GRA_BindUniformBuffer(pCmd, color, sizeof(float) * 4);

    constexpr uint32_t stride = sizeof(float) * 3;
    GRA_BindVertexBuffer(pCmd, beamvertex, sizeof(beamvertex), stride);
    cmdDraw(pCmd, NUM_BEAM_SEGS * 4, 0);
}

//===================================================================

void R_BeginRegistration(char *map);
struct model_s *R_RegisterModel(char *name);
struct image_s *R_RegisterSkin(char *name);
void R_SetSky(char *name, float rotate, vec3_t axis);
void R_EndRegistration(void);

void R_RenderFrame(refdef_t *fd);

struct image_s *Draw_FindPic(char *name);

void Draw_Pic(int x, int y, char *name);
void Draw_Char(int x, int y, int c);
void Draw_TileClear(int x, int y, int w, int h, char *name);
void Draw_Fill(int x, int y, int w, int h, int c);
void Draw_FadeScreen(void);

/*
@@@@@@@@@@@@@@@@@@@@@
GetRefAPI

@@@@@@@@@@@@@@@@@@@@@
*/
void refreshExport()
{
    re.api_version = API_VERSION;

    re.BeginRegistration = R_BeginRegistration;
    re.RegisterModel = R_RegisterModel;
    re.RegisterSkin = R_RegisterSkin;
    re.RegisterPic = Draw_FindPic;
    re.SetSky = R_SetSky;

    re.EndRegistration = R_EndRegistration;

    re.RenderFrame = R_RenderFrame;

    re.DrawGetPicSize = [](int *w, int *h, char *pic) { Draw_GetPicSize(w, h, pic); };
    re.DrawPic = Draw_Pic;
    re.DrawStretchPic = Draw_StretchPic;
    re.DrawChar = Draw_Char;
    re.DrawTileClear = Draw_TileClear;
    re.DrawFill = Draw_Fill;
    re.DrawFadeScreen = Draw_FadeScreen;

    re.DrawStretchRaw = Draw_StretchRaw;

    // re.Init = R_Init;
    re.Shutdown = R_Shutdown;

    re.CinematicSetPalette = R_SetPalette;
    re.BeginFrame = R_BeginFrame;
    re.EndFrame = R_EndFrame;
    re.EndWorldRenderpass = R_EndWorldRenderpass;

    re.AppActivate = [](qboolean) { LOGF(eDEBUG, "Vkimp_AppActivate"); };

    // Swap_Init();
}

void Mat_Identity(float *matrix)
{
    matrix[0] = 1.f;
    matrix[1] = 0.f;
    matrix[2] = 0.f;
    matrix[3] = 0.f;
    matrix[4] = 0.f;
    matrix[5] = 1.f;
    matrix[6] = 0.f;
    matrix[7] = 0.f;
    matrix[8] = 0.f;
    matrix[9] = 0.f;
    matrix[10] = 1.f;
    matrix[11] = 0.f;
    matrix[12] = 0.f;
    matrix[13] = 0.f;
    matrix[14] = 0.f;
    matrix[15] = 1.f;
}

void Mat_Mul(float *m1, float *m2, float *res)
{
    float mul[16] = {m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12],
                     m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13],
                     m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14],
                     m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15],
                     m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12],
                     m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13],
                     m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14],
                     m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15],
                     m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12],
                     m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13],
                     m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14],
                     m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15],
                     m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12],
                     m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13],
                     m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14],
                     m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15]};

    memcpy(res, mul, sizeof(float) * 16);
}

void Mat_Translate(float *matrix, float x, float y, float z)
{
    float t[16] = {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, x, y, z, 1.f};

    Mat_Mul(matrix, t, matrix);
}

void Mat_Rotate(float *matrix, float deg, float x, float y, float z)
{
    double c = cos(deg * M_PI / 180.0);
    double s = sin(deg * M_PI / 180.0);
    double cd = 1.0 - c;
    vec3_t r = {x, y, z};
    VectorNormalize(r);

    float rot[16] = {
        static_cast<float>(r[0] * r[0] * cd + c),
        static_cast<float>(r[1] * r[0] * cd + r[2] * s),
        static_cast<float>(r[0] * r[2] * cd - r[1] * s),
        0.f,
        static_cast<float>(r[0] * r[1] * cd - r[2] * s),
        static_cast<float>(r[1] * r[1] * cd + c),
        static_cast<float>(r[1] * r[2] * cd + r[0] * s),
        0.f,
        static_cast<float>(r[0] * r[2] * cd + r[1] * s),
        static_cast<float>(r[1] * r[2] * cd - r[0] * s),
        static_cast<float>(r[2] * r[2] * cd + c),
        0.f,
        0.f,
        0.f,
        0.f,
        1.f,
    };

    Mat_Mul(matrix, rot, matrix);
}

void Mat_Scale(float *matrix, float x, float y, float z)
{
    float s[16] = {x, 0.f, 0.f, 0.f, 0.f, y, 0.f, 0.f, 0.f, 0.f, z, 0.f, 0.f, 0.f, 0.f, 1.f};

    Mat_Mul(matrix, s, matrix);
}

void Mat_Perspective(float *matrix, float *correction_matrix, float fovy, float aspect, float zNear, float zFar)
{
    float xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;

    xmin = ymin * aspect;
    xmax = ymax * aspect;

    xmin += -(2 * vk_state.camera_separation) / zNear;
    xmax += -(2 * vk_state.camera_separation) / zNear;

    float proj[16];
    memset(proj, 0, sizeof(float) * 16);
    proj[0] = 2.f * zNear / (xmax - xmin);
    proj[2] = (xmax + xmin) / (xmax - xmin);
    proj[5] = 2.f * zNear / (ymax - ymin);
    proj[6] = (ymax + ymin) / (ymax - ymin);
    proj[10] = -(zFar + zNear) / (zFar - zNear);
    proj[11] = -1.f;
    proj[14] = -2.f * zFar * zNear / (zFar - zNear);

    // Convert projection matrix to Vulkan coordinate system
    // (https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/)
    Mat_Mul(proj, correction_matrix, matrix);
}

void Mat_Ortho(float *matrix, float left, float right, float bottom, float top, float zNear, float zFar)
{
    float proj[16];
    memset(proj, 0, sizeof(float) * 16);
    proj[0] = 2.f / (right - left);
    proj[3] = (right + left) / (right - left);
    proj[5] = 2.f / (top - bottom);
    proj[7] = (top + bottom) / (top - bottom);
    proj[10] = -2.f / (zFar - zNear);
    proj[11] = -(zFar + zNear) / (zFar - zNear);
    proj[15] = 1.f;

    // Convert projection matrix to Vulkan coordinate system
    // (https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/)
    Mat_Mul(proj, toFloatPtr(r_vulkan_correction), matrix);
}
