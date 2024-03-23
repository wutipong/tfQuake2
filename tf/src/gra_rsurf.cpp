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
// vk_rsurf.c: surface-related refresh code
#include <assert.h>

#include "gra_common.h"
#include "gra_local.h"
#include <array>
#include <format>

extern model_t *currentmodel;
extern model_t *r_worldmodel;

static vec3 modelorg; // relative to viewpoint

msurface_t *r_alpha_surfaces;

constexpr int DYNAMIC_LIGHT_WIDTH = 128;
constexpr int DYNAMIC_LIGHT_HEIGHT = 128;

constexpr int LIGHTMAP_BYTES = 4;

constexpr int BLOCK_WIDTH = 128;
constexpr int BLOCK_HEIGHT = 128;

int c_visible_lightmaps;
int c_visible_textures;

typedef struct
{
    int current_lightmap_texture;

    std::array<msurface_t *, MAX_LIGHTMAPS> lightmap_surfaces;

    std::array<int, BLOCK_WIDTH> allocated;

    // the lightmap texture data needs to be kept in
    // main memory so texsubimage can update properly
    std::array<byte, 4 * BLOCK_WIDTH * BLOCK_HEIGHT> lightmap_buffer;
} vklightmapstate_t;

static vklightmapstate_t vk_lms;

static void LM_InitBlock(void);
static void LM_UploadBlock(qboolean dynamic);
static qboolean LM_AllocBlock(int w, int h, int *x, int *y);

extern void R_SetCacheState(msurface_t *surf);
extern void R_BuildLightMap(msurface_t *surf, byte *dest, int stride);

/*
=============================================================

    BRUSH MODELS

=============================================================
*/

/*
===============
R_TextureAnimation

Returns the proper texture for a given time and base texture
===============
*/
image_t *R_TextureAnimation(mtexinfo_t *tex)
{
    int c;

    if (!tex->next)
        return tex->image;

    c = currententity->frame % tex->numframes;
    while (c)
    {
        tex = tex->next;
        c--;
    }

    return tex->image;
}

/*
================
DrawVkPoly
================
*/
void DrawVkPoly(vkpoly_t *p, image_t *texture, vec4 color)
{
    int i;
    float *v;

    typedef struct
    {
        vec3 vertex;
        vec2 texCoord;
    } polyvert;

    static polyvert verts[MAX_VERTS];

    v = p->verts[0];
    for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
    {
        verts[i].vertex = {v[0], v[1], v[2]};
        verts[i].texCoord = {v[3], v[4]};
    }

    cmdBindPipeline(pCmd, drawPolyPipeline);
    cmdBindDescriptorSet(pCmd, 0, pDSTexture[texture->index]);
    cmdBindDescriptorSet(pCmd, 0, pDSUniform);

    uint32_t stride = sizeof(polyvert);
    GRA_BindVertexBuffer(pCmd, verts, sizeof(polyvert) * p->numverts, stride);
    // (pCmd, pRootSignature, gPushConstantSmall, &color);
    GRA_BindUniformBuffer(pCmd, pDSDynamicUniforms, &color, sizeof(color));

    auto indexCount = GRA_BindTriangleFanIBO(pCmd, p->numverts);
    cmdDrawIndexed(pCmd, indexCount, 0, 0);
}

//============
// PGM
/*
================
DrawVkFlowingPoly -- version of DrawVkPoly that handles scrolling texture
================
*/
void DrawVkFlowingPoly(msurface_t *fa, image_t *texture, vec4 color)
{
    int i;
    float *v;
    vkpoly_t *p;
    float scroll;

    typedef struct
    {
        vec3 vertex;
        vec2 texCoord;
    } polyvert;

    static polyvert verts[MAX_VERTS];

    p = fa->polys;

    scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));
    if (scroll == 0.0)
        scroll = -64.0;

    v = p->verts[0];
    for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
    {
        verts[i].vertex = {v[0], v[1], v[2]};
        verts[i].texCoord = {v[3] + scroll, v[4]};
    }

    cmdBindPipeline(pCmd, drawPolyPipeline);
    // (pCmd, pRootSignature, gPushConstantSmall, &color);
    GRA_BindUniformBuffer(pCmd, pDSDynamicUniforms, &color, sizeof(color));

    uint32_t stride = sizeof(polyvert);
    GRA_BindVertexBuffer(pCmd, verts, sizeof(polyvert) * p->numverts, stride);
    cmdBindDescriptorSet(pCmd, 0, pDSUniform);
    cmdBindDescriptorSet(pCmd, 0, pDSTexture[texture->index]);
    auto indexCount = GRA_BindTriangleFanIBO(pCmd, p->numverts);
    cmdDrawIndexed(pCmd, indexCount, 0, 0);
}
// PGM
//============

/*
** R_DrawTriangleOutlines
*/
void R_DrawTriangleOutlines(void)
{
    int i, j, k;
    vkpoly_t *p;

    if (!vk_showtris->value)
        return;

    VkBuffer vbo;
    VkDeviceSize vboOffset;
    float color[3] = {1.f, 1.f, 1.f};
    struct
    {
        vec3_t v;
        float color[3];
    } triVert[4];

    cmdBindPipeline(pCmd, showTrisPipeline);
    // (pCmd, pRootSignature, gPushConstantSmall, color);
    GRA_BindUniformBuffer(pCmd, pDSDynamicUniforms, color, sizeof(float) * 3);

    for (i = 0; i < MAX_LIGHTMAPS; i++)
    {
        msurface_t *surf;

        for (surf = vk_lms.lightmap_surfaces[i]; surf != 0; surf = surf->lightmapchain)
        {
            p = surf->polys;
            for (; p; p = p->chain)
            {
                for (j = 2, k = 0; j < p->numverts; j++, k++)
                {
                    triVert[0].v[0] = p->verts[0][0];
                    triVert[0].v[1] = p->verts[0][1];
                    triVert[0].v[2] = p->verts[0][2];
                    memcpy(triVert[0].color, color, sizeof(color));

                    triVert[1].v[0] = p->verts[j - 1][0];
                    triVert[1].v[1] = p->verts[j - 1][1];
                    triVert[1].v[2] = p->verts[j - 1][2];
                    memcpy(triVert[1].color, color, sizeof(color));

                    triVert[2].v[0] = p->verts[j][0];
                    triVert[2].v[1] = p->verts[j][1];
                    triVert[2].v[2] = p->verts[j][2];
                    memcpy(triVert[2].color, color, sizeof(color));

                    triVert[3].v[0] = p->verts[0][0];
                    triVert[3].v[1] = p->verts[0][1];
                    triVert[3].v[2] = p->verts[0][2];
                    memcpy(triVert[3].color, color, sizeof(color));

                    uint32_t stride = sizeof(float) * 6;
                    GRA_BindVertexBuffer(pCmd, triVert, sizeof(triVert), stride);
                    cmdDraw(pCmd, 4, 0);
                }
            }
        }
    }
}

/*
================
R_RenderBrushPoly
================
*/
void R_RenderBrushPoly(msurface_t *fa, float *modelMatrix, float alpha)
{
    int maps;
    image_t *image;
    qboolean is_dynamic = false;
    vec4 color = {1.f, 1.f, 1.f, alpha};
    c_brush_polys++;

    image = R_TextureAnimation(fa->texinfo);

    if (fa->flags & SURF_DRAWTURB)
    {
        color[0] = color[1] = color[2] = vk_state.inverse_intensity;
        color[3] = 1.f;
        // warp texture, no lightmaps
        EmitWaterPolys(fa, image, modelMatrix, color);
        return;
    }

    //======
    // PGM
    if (fa->texinfo->flags & SURF_FLOWING)
        DrawVkFlowingPoly(fa, image, color);
    else
        DrawVkPoly(fa->polys, image, color);
    // PGM
    //======

    /*
    ** check for lightmap modification
    */
    for (maps = 0; maps < MAXLIGHTMAPS && fa->styles[maps] != 255; maps++)
    {
        if (r_newrefdef.lightstyles[fa->styles[maps]].white != fa->cached_light[maps])
            goto dynamic;
    }

    // dynamic this frame or dynamic previously
    if (fa->dlightframe == r_framecount)
    {
    dynamic:
        if (vk_dynamic->value)
        {
            if (!(fa->texinfo->flags & (SURF_SKY | SURF_TRANS33 | SURF_TRANS66 | SURF_WARP)))
            {
                is_dynamic = true;
            }
        }
    }

    if (is_dynamic)
    {
        if ((fa->styles[maps] >= 32 || fa->styles[maps] == 0) && (fa->dlightframe != r_framecount))
        {
            unsigned temp[34 * 34];
            int smax, tmax;

            smax = (fa->extents[0] >> 4) + 1;
            tmax = (fa->extents[1] >> 4) + 1;

            R_BuildLightMap(fa, (byte *)temp, smax * 4);
            R_SetCacheState(fa);

            TextureUpdateDesc updateDesc{
                .pTexture = vk_state.lightmap_textures[fa->lightmaptexturenum],
                .mBaseMipLevel = 0,
                .mMipLevels = 1,
                .mBaseArrayLayer = 0,
                .mLayerCount = 1,
                .mCurrentState = RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            };

            beginUpdateResource(&updateDesc);

            TextureSubresourceUpdate subresource = updateDesc.getSubresourceUpdateDesc(0, 0);
            memcpy(subresource.pMappedData, (unsigned char *)temp, sizeof(temp));
            endUpdateResource(&updateDesc);

            fa->lightmapchain = vk_lms.lightmap_surfaces[fa->lightmaptexturenum];
            vk_lms.lightmap_surfaces[fa->lightmaptexturenum] = fa;
        }
        else
        {
            fa->lightmapchain = vk_lms.lightmap_surfaces[0];
            vk_lms.lightmap_surfaces[0] = fa;
        }
    }
    else
    {
        fa->lightmapchain = vk_lms.lightmap_surfaces[fa->lightmaptexturenum];
        vk_lms.lightmap_surfaces[fa->lightmaptexturenum] = fa;
    }
}

/*
================
R_DrawAlphaSurfaces

Draw water surfaces and windows.
The BSP tree is waled front to back, so unwinding the chain
of alpha_surfaces will draw back to front, giving proper ordering.
================
*/
void R_DrawAlphaSurfaces(void)
{
    msurface_t *s;
    float intens;

    // the textures are prescaled up for a better lighting range,
    // so scale it back down
    intens = vk_state.inverse_intensity;
    vec4 color = {intens, intens, intens, 1.f};

    for (s = r_alpha_surfaces; s; s = s->texturechain)
    {
        c_brush_polys++;
        if (s->texinfo->flags & SURF_TRANS33)
            color[3] = 0.33f;
        else if (s->texinfo->flags & SURF_TRANS66)
            color[3] = 0.66f;

        if (s->flags & SURF_DRAWTURB)
            EmitWaterPolys(s, s->texinfo->image, NULL, color);
        else if (s->texinfo->flags & SURF_FLOWING)          // PGM	9/16/98
            DrawVkFlowingPoly(s, s->texinfo->image, color); // PGM
        else
            DrawVkPoly(s->polys, s->texinfo->image, color);
    }

    r_alpha_surfaces = NULL;
}

/*
================
DrawTextureChains
================
*/
void DrawTextureChains(void)
{
    int i;
    msurface_t *s;
    image_t *image;

    c_visible_textures = 0;

    for (i = 0, image = vktextures; i < numvktextures; i++, image++)
    {
        if (!image->registration_sequence)
            continue;
        if (!image->texturechain)
            continue;
        c_visible_textures++;

        for (s = image->texturechain; s; s = s->texturechain)
        {
            if (!(s->flags & SURF_DRAWTURB))
                R_RenderBrushPoly(s, NULL, 1.f);
        }
    }

    for (i = 0, image = vktextures; i < numvktextures; i++, image++)
    {
        if (!image->registration_sequence)
            continue;
        s = image->texturechain;
        if (!s)
            continue;

        for (; s; s = s->texturechain)
        {
            if (s->flags & SURF_DRAWTURB)
                R_RenderBrushPoly(s, NULL, 1.f);
        }

        image->texturechain = NULL;
    }
}

static void Vk_RenderLightmappedPoly(msurface_t *surf, float *modelMatrix, float alpha)
{
    int i, nv = surf->polys->numverts;
    int map;
    float *v;
    image_t *image = R_TextureAnimation(surf->texinfo);
    qboolean is_dynamic = false;
    unsigned lmtex = surf->lightmaptexturenum;
    vkpoly_t *p;

    struct lmappolyvert
    {
        alignas(vec4) vec3 vertex;
        vec2 texCoord;
        vec2 texCoordLmap;
    };

    static lmappolyvert verts[MAX_VERTS];

    struct
    {
        mat4 model;
        float viewLightmaps;
    } lmapPolyUbo;

    lmapPolyUbo.viewLightmaps = vk_lightmap->value ? 1.f : 0.f;

    if (modelMatrix)
    {
        lmapPolyUbo.model = {
            {modelMatrix[0], modelMatrix[1], modelMatrix[2], modelMatrix[3]},
            {modelMatrix[4], modelMatrix[5], modelMatrix[6], modelMatrix[7]},
            {modelMatrix[8], modelMatrix[9], modelMatrix[10], modelMatrix[11]},
            {modelMatrix[12], modelMatrix[13], modelMatrix[14], modelMatrix[15]},
        };
    }
    else
    {
        lmapPolyUbo.model = mat4::identity();
    }

    cmdBindPipeline(pCmd, drawPolyLmapPipeline);
    cmdBindDescriptorSet(pCmd, 0, pDSUniformModel);
    //(pCmd, pRootSignature, gPushConstantLarge, &lmapPolyUbo);
    GRA_BindUniformBuffer(pCmd, pDSDynamicUniformsModel, &lmapPolyUbo, sizeof(lmapPolyUbo));
    

    for (map = 0; map < MAXLIGHTMAPS && surf->styles[map] != 255; map++)
    {
        if (r_newrefdef.lightstyles[surf->styles[map]].white != surf->cached_light[map])
            goto dynamic;
    }

    // dynamic this frame or dynamic previously
    if (surf->dlightframe == r_framecount)
    {
    dynamic:
        if (vk_dynamic->value)
        {
            if (!(surf->texinfo->flags & (SURF_SKY | SURF_TRANS33 | SURF_TRANS66 | SURF_WARP)))
            {
                is_dynamic = true;
            }
        }
    }

    if (is_dynamic)
    {
        unsigned temp[128 * 128];
        int smax, tmax;

        if ((surf->styles[map] >= 32 || surf->styles[map] == 0) && (surf->dlightframe != r_framecount))
        {
            smax = (surf->extents[0] >> 4) + 1;
            tmax = (surf->extents[1] >> 4) + 1;

            R_BuildLightMap(surf, (byte *)temp, smax * 4);
            R_SetCacheState(surf);

            lmtex = surf->lightmaptexturenum;

            TextureUpdateDesc updateDesc{
                .pTexture = vk_state.lightmap_textures[surf->lightmaptexturenum],
                .mBaseMipLevel = 0,
                .mMipLevels = 1,
                .mBaseArrayLayer = 0,
                .mLayerCount = 1,
                .mCurrentState = RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            };

            beginUpdateResource(&updateDesc);

            TextureSubresourceUpdate subresource = updateDesc.getSubresourceUpdateDesc(0, 0);
            memcpy(subresource.pMappedData, (unsigned char *)temp, sizeof(temp));
            endUpdateResource(&updateDesc);
        }
        else
        {
            smax = (surf->extents[0] >> 4) + 1;
            tmax = (surf->extents[1] >> 4) + 1;

            R_BuildLightMap(surf, (byte *)temp, smax * 4);

            lmtex = surf->lightmaptexturenum + DYNLIGHTMAP_OFFSET;
            TextureUpdateDesc updateDesc{
                .pTexture = vk_state.lightmap_textures[lmtex],
                .mBaseMipLevel = 0,
                .mMipLevels = 1,
                .mBaseArrayLayer = 0,
                .mLayerCount = 1,
                .mCurrentState = RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            };

            beginUpdateResource(&updateDesc);

            TextureSubresourceUpdate subresource = updateDesc.getSubresourceUpdateDesc(0, 0);
            memcpy(subresource.pMappedData, (unsigned char *)temp, sizeof(temp));
            endUpdateResource(&updateDesc);
        }

        c_brush_polys++;

        //==========
        // PGM
        if (surf->texinfo->flags & SURF_FLOWING)
        {
            float scroll;

            scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));
            if (scroll == 0.0)
                scroll = -64.0;

            cmdBindDescriptorSet(pCmd, 0, pDSTextureModel[image->index]);
            cmdBindDescriptorSet(pCmd, 0, pDSLightMap[lmtex]);

            for (p = surf->polys; p; p = p->chain)
            {
                v = p->verts[0];
                for (i = 0; i < nv; i++, v += VERTEXSIZE)
                {
                    verts[i].vertex = {v[0], v[1], v[2]};
                    verts[i].texCoord = {v[3] + scroll, v[4]};
                    verts[i].texCoordLmap = {v[5], v[6]};
                }

                constexpr uint32_t stride = sizeof(lmappolyvert);
                GRA_BindVertexBuffer(pCmd, verts, sizeof(lmappolyvert) * nv, stride);

                auto indexCount = GRA_BindTriangleFanIBO(pCmd, nv);
                cmdDrawIndexed(pCmd, indexCount, 0, 0);
            }
        }
        else
        {
            cmdBindDescriptorSet(pCmd, 0, pDSTextureModel[image->index]);
            cmdBindDescriptorSet(pCmd, 0, pDSLightMap[lmtex]);

            for (p = surf->polys; p; p = p->chain)
            {
                v = p->verts[0];
                for (i = 0; i < nv; i++, v += VERTEXSIZE)
                {
                    verts[i].vertex = {v[0], v[1], v[2]};
                    verts[i].texCoord = {v[3], v[4]};
                    verts[i].texCoordLmap = {v[5], v[6]};
                }

                constexpr uint32_t stride = sizeof(lmappolyvert);
                GRA_BindVertexBuffer(pCmd, verts, sizeof(lmappolyvert) * nv, stride);
                auto indexCount = GRA_BindTriangleFanIBO(pCmd, nv);
                cmdDrawIndexed(pCmd, indexCount, 0, 0);
            }
        }
        // PGM
        //==========
    }
    else
    {
        c_brush_polys++;

        //==========
        // PGM
        if (surf->texinfo->flags & SURF_FLOWING)
        {
            float scroll;

            scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));
            if (scroll == 0.0)
                scroll = -64.0;

            for (p = surf->polys; p; p = p->chain)
            {
                v = p->verts[0];
                for (i = 0; i < nv; i++, v += VERTEXSIZE)
                {
                    verts[i].vertex = {v[0], v[1], v[2]};
                    verts[i].texCoord = {v[3] + scroll, v[4]};
                    verts[i].texCoordLmap = {v[5], v[6]};
                }

                constexpr uint32_t stride = sizeof(lmappolyvert);
                GRA_BindVertexBuffer(pCmd, verts, sizeof(lmappolyvert) * nv, stride);
                cmdBindDescriptorSet(pCmd, 0, pDSTextureModel[image->index]);
                cmdBindDescriptorSet(pCmd, 0, pDSLightMap[lmtex]);
                auto indexCount = GRA_BindTriangleFanIBO(pCmd, nv);

                cmdDrawIndexed(pCmd, indexCount, 0, 0);
            }
        }
        else
        {
            // PGM
            //==========
            for (p = surf->polys; p; p = p->chain)
            {
                v = p->verts[0];
                for (i = 0; i < nv; i++, v += VERTEXSIZE)
                {
                    verts[i].vertex = {v[0], v[1], v[2]};
                    verts[i].texCoord = {v[3], v[4]};
                    verts[i].texCoordLmap = {v[5], v[6]};
                }

                constexpr uint32_t stride = sizeof(lmappolyvert);
                GRA_BindVertexBuffer(pCmd, verts, sizeof(lmappolyvert) * nv, stride);
                auto indexCount = GRA_BindTriangleFanIBO(pCmd, nv);
                cmdBindDescriptorSet(pCmd, 0, pDSTextureModel[image->index]);
                cmdBindDescriptorSet(pCmd, 0, pDSLightMap[lmtex]);
                cmdDrawIndexed(pCmd, indexCount, 0, 0);
            }
            //==========
            // PGM
        }
        // PGM
        //==========
    }
}

/*
=================
R_DrawInlineBModel
=================
*/
void R_DrawInlineBModel(float *modelMatrix)
{
    int i, k;
    msurface_t *psurf;
    dlight_t *lt;
    float alpha = 1.f;

    // calculate dynamic lighting for bmodel

    if (!vk_flashblend->value)
    {
        lt = r_newrefdef.dlights;
        for (k = 0; k < r_newrefdef.num_dlights; k++, lt++)
        {
            R_MarkLights(lt, 1 << k, currentmodel->nodes + currentmodel->firstnode);
        }
    }

    psurf = &currentmodel->surfaces[currentmodel->firstmodelsurface];

    if (currententity->flags & RF_TRANSLUCENT)
    {
        alpha = .25f;
    }

    //
    // draw texture
    //
    for (i = 0; i < currentmodel->nummodelsurfaces; i++, psurf++)
    {
        // find which side of the node we are on
        cplane_t *pplane = psurf->plane;
        vec3 normal = {pplane->normal[0], pplane->normal[1], pplane->normal[2]};
        float d = dot(modelorg, normal) - pplane->dist;

        // draw the polygon
        if (((psurf->flags & SURF_PLANEBACK) && (d < -BACKFACE_EPSILON)) ||
            (!(psurf->flags & SURF_PLANEBACK) && (d > BACKFACE_EPSILON)))
        {
            if (psurf->texinfo->flags & (SURF_TRANS33 | SURF_TRANS66))
            { // add to the translucent chain
                psurf->texturechain = r_alpha_surfaces;
                r_alpha_surfaces = psurf;
            }
            else if (!(psurf->flags & SURF_DRAWTURB) && !vk_showtris->value)
            {
                Vk_RenderLightmappedPoly(psurf, modelMatrix, alpha);
            }
            else
            {
                R_RenderBrushPoly(psurf, modelMatrix, alpha);
            }
        }
    }
}

/*
=================
R_DrawBrushModel
=================
*/
void R_DrawBrushModel(entity_t *e)
{
    vec3_t mins, maxs;
    int i;
    qboolean rotated;

    if (currentmodel->nummodelsurfaces == 0)
        return;

    currententity = e;

    if (e->angles[0] || e->angles[1] || e->angles[2])
    {
        rotated = true;
        for (i = 0; i < 3; i++)
        {
            mins[i] = e->origin[i] - currentmodel->radius;
            maxs[i] = e->origin[i] + currentmodel->radius;
        }
    }
    else
    {
        rotated = false;
        VectorAdd(e->origin, currentmodel->mins, mins);
        VectorAdd(e->origin, currentmodel->maxs, maxs);
    }

    if (R_CullBox(mins, maxs))
        return;

    vk_lms.lightmap_surfaces.fill(0);

    VectorSubtract(r_newrefdef.vieworg, e->origin, modelorg);
    if (rotated)
    {
        vec3_t temp;
        vec3_t forward, right, up;

        VectorCopy(modelorg, temp);
        AngleVectors(e->angles, forward, right, up);
        modelorg[0] = DotProduct(temp, forward);
        modelorg[1] = -DotProduct(temp, right);
        modelorg[2] = DotProduct(temp, up);
    }

    e->angles[0] = -e->angles[0]; // stupid quake bug
    e->angles[2] = -e->angles[2]; // stupid quake bug
    float model[16];
    Mat_Identity(model);
    R_RotateForEntity(e, model);
    e->angles[0] = -e->angles[0]; // stupid quake bug
    e->angles[2] = -e->angles[2]; // stupid quake bug

    R_DrawInlineBModel(model);
}

/*
=============================================================

    WORLD MODEL

=============================================================
*/

/*
================
R_RecursiveWorldNode
================
*/
void R_RecursiveWorldNode(mnode_t *node)
{
    int c, side, sidebit;
    cplane_t *plane;
    msurface_t *surf, **mark;
    mleaf_t *pleaf;
    float dot;
    image_t *image;

    if (node->contents == CONTENTS_SOLID)
        return; // solid

    if (node->visframe != r_visframecount)
        return;
    if (R_CullBox(node->minmaxs, node->minmaxs + 3))
        return;

    // if a leaf node, draw stuff
    if (node->contents != -1)
    {
        pleaf = (mleaf_t *)node;

        // check for door connected areas
        if (r_newrefdef.areabits)
        {
            if (!(r_newrefdef.areabits[pleaf->area >> 3] & (1 << (pleaf->area & 7))))
                return; // not visible
        }

        mark = pleaf->firstmarksurface;
        c = pleaf->nummarksurfaces;

        if (c)
        {
            do
            {
                (*mark)->visframe = r_framecount;
                mark++;
            } while (--c);
        }

        return;
    }

    // node is just a decision point, so go down the apropriate sides

    // find which side of the node we are on
    plane = node->plane;

    switch (plane->type)
    {
    case PLANE_X:
        dot = modelorg[0] - plane->dist;
        break;
    case PLANE_Y:
        dot = modelorg[1] - plane->dist;
        break;
    case PLANE_Z:
        dot = modelorg[2] - plane->dist;
        break;
    default:
        dot = DotProduct(modelorg, plane->normal) - plane->dist;
        break;
    }

    if (dot >= 0)
    {
        side = 0;
        sidebit = 0;
    }
    else
    {
        side = 1;
        sidebit = SURF_PLANEBACK;
    }

    // recurse down the children, front side first
    R_RecursiveWorldNode(node->children[side]);

    // draw stuff
    for (c = node->numsurfaces, surf = r_worldmodel->surfaces + node->firstsurface; c; c--, surf++)
    {
        if (surf->visframe != r_framecount)
            continue;

        if ((surf->flags & SURF_PLANEBACK) != sidebit)
            continue; // wrong side

        if (surf->texinfo->flags & SURF_SKY)
        { // just adds to visible sky bounds
            R_AddSkySurface(surf);
        }
        else if (surf->texinfo->flags & (SURF_TRANS33 | SURF_TRANS66))
        { // add to the translucent chain
            surf->texturechain = r_alpha_surfaces;
            r_alpha_surfaces = surf;
        }
        else
        {
            if (!(surf->flags & SURF_DRAWTURB) && !vk_showtris->value)
            {
                Vk_RenderLightmappedPoly(surf, NULL, 1.f);
            }
            else
            {
                // the polygon is visible, so add it to the texture
                // sorted chain
                // FIXME: this is a hack for animation
                image = R_TextureAnimation(surf->texinfo);
                surf->texturechain = image->texturechain;
                image->texturechain = surf;
            }
        }
    }

    // recurse down the back side
    R_RecursiveWorldNode(node->children[!side]);
}

/*
=============
R_DrawWorld
=============
*/
void R_DrawWorld(void)
{
    entity_t ent;

    if (!r_drawworld->value)
        return;

    if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
        return;

    currentmodel = r_worldmodel;

    VectorCopy(r_newrefdef.vieworg, modelorg);

    // auto cycle the world frame for texture animation
    memset(&ent, 0, sizeof(ent));
    ent.frame = (int)(r_newrefdef.time * 2);
    currententity = &ent;

    vk_lms.lightmap_surfaces.fill(0);
    R_ClearSkyBox();

    R_RecursiveWorldNode(r_worldmodel->nodes);

    /*
    ** theoretically nothing should happen in the next two functions
    ** if multitexture is enabled - in practice, this code renders non-transparent liquids!
    */
    DrawTextureChains();

    R_DrawSkyBox();

    R_DrawTriangleOutlines();
}

/*
===============
R_MarkLeaves

Mark the leaves and nodes that are in the PVS for the current
cluster
===============
*/
void R_MarkLeaves(void)
{
    byte *vis;
    byte fatvis[MAX_MAP_LEAFS / 8];
    mnode_t *node;
    int i, c;
    mleaf_t *leaf;
    int cluster;

    if (r_oldviewcluster == r_viewcluster && r_oldviewcluster2 == r_viewcluster2 && !r_novis->value &&
        r_viewcluster != -1)
        return;

    // development aid to let you run around and see exactly where
    // the pvs ends
    if (vk_lockpvs->value)
        return;

    r_visframecount++;
    r_oldviewcluster = r_viewcluster;
    r_oldviewcluster2 = r_viewcluster2;

    if (r_novis->value || r_viewcluster == -1 || !r_worldmodel->vis)
    {
        // mark everything
        for (i = 0; i < r_worldmodel->numleafs; i++)
            r_worldmodel->leafs[i].visframe = r_visframecount;
        for (i = 0; i < r_worldmodel->numnodes; i++)
            r_worldmodel->nodes[i].visframe = r_visframecount;
        return;
    }

    vis = Mod_ClusterPVS(r_viewcluster, r_worldmodel);
    // may have to combine two clusters because of solid water boundaries
    if (r_viewcluster2 != r_viewcluster)
    {
        memcpy(fatvis, vis, (r_worldmodel->numleafs + 7) / 8);
        vis = Mod_ClusterPVS(r_viewcluster2, r_worldmodel);
        c = (r_worldmodel->numleafs + 31) / 32;
        for (i = 0; i < c; i++)
            ((int *)fatvis)[i] |= ((int *)vis)[i];
        vis = fatvis;
    }

    for (i = 0, leaf = r_worldmodel->leafs; i < r_worldmodel->numleafs; i++, leaf++)
    {
        cluster = leaf->cluster;
        if (cluster == -1)
            continue;
        if (vis[cluster >> 3] & (1 << (cluster & 7)))
        {
            node = (mnode_t *)leaf;
            do
            {
                if (node->visframe == r_visframecount)
                    break;
                node->visframe = r_visframecount;
                node = node->parent;
            } while (node);
        }
    }
}

/*
=============================================================================

  LIGHTMAP ALLOCATION

=============================================================================
*/

static void LM_InitBlock(void)
{
    vk_lms.allocated.fill(0);
}

static void LM_UploadBlock(qboolean dynamic)
{
    int texture;
    int height = 0;

    if (dynamic)
    {
        texture = 0;
    }
    else
    {
        texture = vk_lms.current_lightmap_texture;
    }

    if (dynamic)
    {
        int i;

        for (i = 0; i < BLOCK_WIDTH; i++)
        {
            if (vk_lms.allocated[i] > height)
                height = vk_lms.allocated[i];
        }

        TextureUpdateDesc updateDesc{
            .pTexture = vk_state.lightmap_textures[texture],
            .mBaseMipLevel = 0,
            .mMipLevels = 1,
            .mBaseArrayLayer = 0,
            .mLayerCount = 1,
            .mCurrentState = RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        };

        beginUpdateResource(&updateDesc);

        TextureSubresourceUpdate subresource = updateDesc.getSubresourceUpdateDesc(0, 0);
        memcpy(subresource.pMappedData, vk_lms.lightmap_buffer.data(), BLOCK_WIDTH * height * sizeof(uint32_t));
        endUpdateResource(&updateDesc);
    }
    else
    {
        if (vk_state.lightmap_textures[texture] != NULL)
        {
            TextureUpdateDesc updateDesc{
                .pTexture = vk_state.lightmap_textures[texture],
                .mBaseMipLevel = 0,
                .mMipLevels = 1,
                .mBaseArrayLayer = 0,
                .mLayerCount = 1,
                .mCurrentState = RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            };

            beginUpdateResource(&updateDesc);

            TextureSubresourceUpdate subresource = updateDesc.getSubresourceUpdateDesc(0, 0);
            memcpy(subresource.pMappedData, vk_lms.lightmap_buffer.data(),
                   BLOCK_WIDTH * BLOCK_HEIGHT * sizeof(uint32_t));
            endUpdateResource(&updateDesc);
        }
        else
        {
            SyncToken token = {};

            TextureDesc textureDesc = {
                .pName = std::format("Image: dynamic lightmap #{}", texture).c_str(),
                .mWidth = BLOCK_WIDTH,
                .mHeight = BLOCK_HEIGHT,
                .mDepth = 1,
                .mArraySize = 1,
                .mMipLevels = 1,
                .mSampleCount = SAMPLE_COUNT_1,
                .mFormat = TinyImageFormat_R8G8B8A8_UNORM,
                .mStartState = RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                .mDescriptors = DESCRIPTOR_TYPE_TEXTURE,
            };

            TextureLoadDesc loadDesc{};
            loadDesc.pDesc = &textureDesc;
            loadDesc.ppTexture = &vk_state.lightmap_textures[texture];

            addResource(&loadDesc, &token);

            waitForToken(&token);

            TextureUpdateDesc updateDesc{
                .pTexture = vk_state.lightmap_textures[texture],
                .mBaseMipLevel = 0,
                .mMipLevels = 1,
                .mBaseArrayLayer = 0,
                .mLayerCount = 1,
                .mCurrentState = RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            };

            beginUpdateResource(&updateDesc);

            TextureSubresourceUpdate subresource = updateDesc.getSubresourceUpdateDesc(0, 0);
            memcpy(subresource.pMappedData, vk_lms.lightmap_buffer.data(),
                   BLOCK_WIDTH * BLOCK_HEIGHT * sizeof(uint32_t));
            endUpdateResource(&updateDesc);

            DescriptorData paramsTex = {
                .pName = "sLightmap",
                .mIndex = getDescriptorIndexFromName(pRootSignature, "sLightmap"),
                .ppTextures = &vk_state.lightmap_textures[texture],
            };
            updateDescriptorSet(pRenderer, 0, pDSLightMap[texture], 1, &paramsTex);
        }
        if (++vk_lms.current_lightmap_texture == MAX_LIGHTMAPS)
        {
            LOGF(eERROR, "LM_UploadBlock() - MAX_LIGHTMAPS exceeded.");
        }
    }
}

// returns a texture number and the position inside it
static qboolean LM_AllocBlock(int w, int h, int *x, int *y)
{
    int i, j;
    int best, best2;

    best = BLOCK_HEIGHT;

    for (i = 0; i < BLOCK_WIDTH - w; i++)
    {
        best2 = 0;

        for (j = 0; j < w; j++)
        {
            if (vk_lms.allocated[i + j] >= best)
                break;
            if (vk_lms.allocated[i + j] > best2)
                best2 = vk_lms.allocated[i + j];
        }
        if (j == w)
        { // this is a valid spot
            *x = i;
            *y = best = best2;
        }
    }

    if (best + h > BLOCK_HEIGHT)
        return false;

    for (i = 0; i < w; i++)
        vk_lms.allocated[*x + i] = best + h;

    return true;
}

/*
================
Vk_BuildPolygonFromSurface
================
*/
void Vk_BuildPolygonFromSurface(msurface_t *fa)
{
    int i, lindex, lnumverts;
    medge_t *pedges, *r_pedge;
    float *vec;
    float s, t;
    vkpoly_t *poly;
    vec3_t total;

    // reconstruct the polygon
    pedges = currentmodel->edges;
    lnumverts = fa->numedges;

    VectorClear(total);
    //
    // draw texture
    //
    poly = (vkpoly_t *)Hunk_Alloc(sizeof(vkpoly_t) + (lnumverts - 4) * VERTEXSIZE * sizeof(float));
    poly->next = fa->polys;
    poly->flags = fa->flags;
    fa->polys = poly;
    poly->numverts = lnumverts;

    for (i = 0; i < lnumverts; i++)
    {
        lindex = currentmodel->surfedges[fa->firstedge + i];

        if (lindex > 0)
        {
            r_pedge = &pedges[lindex];
            vec = currentmodel->vertexes[r_pedge->v[0]].position;
        }
        else
        {
            r_pedge = &pedges[-lindex];
            vec = currentmodel->vertexes[r_pedge->v[1]].position;
        }
        s = DotProduct(vec, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
        s /= fa->texinfo->image->width;

        t = DotProduct(vec, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
        t /= fa->texinfo->image->height;

        VectorAdd(total, vec, total);
        VectorCopy(vec, poly->verts[i]);
        poly->verts[i][3] = s;
        poly->verts[i][4] = t;

        //
        // lightmap texture coordinates
        //
        s = DotProduct(vec, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
        s -= fa->texturemins[0];
        s += fa->light_s * 16;
        s += 8;
        s /= BLOCK_WIDTH * 16; // fa->texinfo->texture->width;

        t = DotProduct(vec, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
        t -= fa->texturemins[1];
        t += fa->light_t * 16;
        t += 8;
        t /= BLOCK_HEIGHT * 16; // fa->texinfo->texture->height;

        poly->verts[i][5] = s;
        poly->verts[i][6] = t;
    }

    poly->numverts = lnumverts;
}

/*
========================
Vk_CreateSurfaceLightmap
========================
*/
void Vk_CreateSurfaceLightmap(msurface_t *surf)
{
    int smax, tmax;
    byte *base;

    if (surf->flags & (SURF_DRAWSKY | SURF_DRAWTURB))
        return;

    smax = (surf->extents[0] >> 4) + 1;
    tmax = (surf->extents[1] >> 4) + 1;

    if (!LM_AllocBlock(smax, tmax, &surf->light_s, &surf->light_t))
    {
        LM_UploadBlock(false);
        LM_InitBlock();
        if (!LM_AllocBlock(smax, tmax, &surf->light_s, &surf->light_t))
        {
            LOGF(eERROR, "Consecutive calls to LM_AllocBlock(%d,%d) failed\n", smax, tmax);
        }
    }

    surf->lightmaptexturenum = vk_lms.current_lightmap_texture;

    base = vk_lms.lightmap_buffer.data();
    base += (surf->light_t * BLOCK_WIDTH + surf->light_s) * LIGHTMAP_BYTES;

    R_SetCacheState(surf);
    R_BuildLightMap(surf, base, BLOCK_WIDTH * LIGHTMAP_BYTES);
}

/*
==================
Vk_BeginBuildingLightmaps

==================
*/
void Vk_BeginBuildingLightmaps(model_t *m)
{
    static lightstyle_t lightstyles[MAX_LIGHTSTYLES];
    int i;
    unsigned dummy[BLOCK_WIDTH * BLOCK_HEIGHT];

    vk_lms.allocated.fill(0);

    r_framecount = 1; // no dlightcache

    /*
    ** setup the base lightstyles so the lightmaps won't have to be regenerated
    ** the first time they're seen
    */
    for (i = 0; i < MAX_LIGHTSTYLES; i++)
    {
        lightstyles[i].rgb[0] = 1;
        lightstyles[i].rgb[1] = 1;
        lightstyles[i].rgb[2] = 1;
        lightstyles[i].white = 3;
    }
    r_newrefdef.lightstyles = lightstyles;

    vk_lms.current_lightmap_texture = 0;

    /*
    ** initialize the dynamic lightmap textures
    */
    if (vk_state.lightmap_textures[DYNLIGHTMAP_OFFSET] == NULL)
    {
        SyncToken token = {};
        for (i = DYNLIGHTMAP_OFFSET; i < MAX_LIGHTMAPS * 2; i++)
        {
            TextureDesc textureDesc = {
                .pName = std::format("Image: dynamic lightmap #{}", i).c_str(),
                .mWidth = BLOCK_WIDTH,
                .mHeight = BLOCK_HEIGHT,
                .mDepth = 1,
                .mArraySize = 1,
                .mMipLevels = 1,
                .mSampleCount = SAMPLE_COUNT_1,
                .mFormat = TinyImageFormat_R8G8B8A8_UNORM,
                .mStartState = RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                .mDescriptors = DESCRIPTOR_TYPE_TEXTURE,
            };

            TextureLoadDesc loadDesc{};
            loadDesc.pDesc = &textureDesc;
            loadDesc.ppTexture = &vk_state.lightmap_textures[i];

            addResource(&loadDesc, &token);
        }

        waitForToken(&token);
    }
}

/*
=======================
Vk_EndBuildingLightmaps
=======================
*/
void Vk_EndBuildingLightmaps(void)
{
    LM_UploadBlock(false);
}
