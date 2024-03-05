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
// vk_warp.c -- sky and water polygons
#include "gra_local.h"
#include "gra_common.h"

#include <format>

extern model_t *loadmodel;

char skyname[MAX_QPATH];
float skyrotate;
vec3_t skyaxis;
image_t *sky_images[6];

msurface_t *warpface;

#define SUBDIVIDE_SIZE 64
// #define	SUBDIVIDE_SIZE	1024

void BoundPoly(int numverts, float *verts, vec3_t mins, vec3_t maxs)
{
    int i, j;
    float *v;

    mins[0] = mins[1] = mins[2] = 9999;
    maxs[0] = maxs[1] = maxs[2] = -9999;
    v = verts;
    for (i = 0; i < numverts; i++)
    {
        for (j = 0; j < 3; j++, v++)
        {
            if (*v < mins[j])
                mins[j] = *v;
            if (*v > maxs[j])
                maxs[j] = *v;
        }
    }
}

// void SubdividePolygon (int numverts, float *verts)
// {
// 	int		i, j, k;
// 	vec3_t	mins, maxs;
// 	float	m;
// 	float	*v;
// 	vec3_t	front[64], back[64];
// 	int		f, b;
// 	float	dist[64];
// 	float	frac;
// 	vkpoly_t	*poly;
// 	float	s, t;
// 	vec3_t	total;
// 	float	total_s, total_t;

// 	if (numverts > 60)
// 		Sys_Error (ERR_DROP, "numverts = %i", numverts);

// 	BoundPoly (numverts, verts, mins, maxs);

// 	for (i=0 ; i<3 ; i++)
// 	{
// 		m = (mins[i] + maxs[i]) * 0.5;
// 		m = SUBDIVIDE_SIZE * floor (m/SUBDIVIDE_SIZE + 0.5);
// 		if (maxs[i] - m < 8)
// 			continue;
// 		if (m - mins[i] < 8)
// 			continue;

// 		// cut it
// 		v = verts + i;
// 		for (j=0 ; j<numverts ; j++, v+= 3)
// 			dist[j] = *v - m;

// 		// wrap cases
// 		dist[j] = dist[0];
// 		v-=i;
// 		VectorCopy (verts, v);

// 		f = b = 0;
// 		v = verts;
// 		for (j=0 ; j<numverts ; j++, v+= 3)
// 		{
// 			if (dist[j] >= 0)
// 			{
// 				VectorCopy (v, front[f]);
// 				f++;
// 			}
// 			if (dist[j] <= 0)
// 			{
// 				VectorCopy (v, back[b]);
// 				b++;
// 			}
// 			if (dist[j] == 0 || dist[j+1] == 0)
// 				continue;
// 			if ( (dist[j] > 0) != (dist[j+1] > 0) )
// 			{
// 				// clip point
// 				frac = dist[j] / (dist[j] - dist[j+1]);
// 				for (k=0 ; k<3 ; k++)
// 					front[f][k] = back[b][k] = v[k] + frac*(v[3+k] - v[k]);
// 				f++;
// 				b++;
// 			}
// 		}

// 		SubdividePolygon (f, front[0]);
// 		SubdividePolygon (b, back[0]);
// 		return;
// 	}

// 	// add a point in the center to help keep warp valid
// 	poly = Hunk_Alloc (sizeof(vkpoly_t) + ((numverts-4)+2) * VERTEXSIZE*sizeof(float));
// 	poly->next = warpface->polys;
// 	warpface->polys = poly;
// 	poly->numverts = numverts+2;
// 	VectorClear (total);
// 	total_s = 0;
// 	total_t = 0;
// 	for (i=0 ; i<numverts ; i++, verts+= 3)
// 	{
// 		VectorCopy (verts, poly->verts[i+1]);
// 		s = DotProduct (verts, warpface->texinfo->vecs[0]);
// 		t = DotProduct (verts, warpface->texinfo->vecs[1]);

// 		total_s += s;
// 		total_t += t;
// 		VectorAdd (total, verts, total);

// 		poly->verts[i+1][3] = s;
// 		poly->verts[i+1][4] = t;
// 	}

// 	VectorScale (total, (1.0/numverts), poly->verts[0]);
// 	poly->verts[0][3] = total_s/numverts;
// 	poly->verts[0][4] = total_t/numverts;

// 	// copy first vertex to last
// 	memcpy (poly->verts[i+1], poly->verts[1], sizeof(poly->verts[0]));
// }

// /*
// ================
// Vk_SubdivideSurface

// Breaks a polygon up along axial 64 unit
// boundaries so that turbulent and sky warps
// can be done reasonably.
// ================
// */
// void Vk_SubdivideSurface (msurface_t *fa)
// {
// 	vec3_t		verts[64];
// 	int			numverts;
// 	int			i;
// 	int			lindex;
// 	float		*vec;

// 	warpface = fa;

// 	//
// 	// convert edges back to a normal polygon
// 	//
// 	numverts = 0;
// 	for (i=0 ; i<fa->numedges ; i++)
// 	{
// 		lindex = loadmodel->surfedges[fa->firstedge + i];

// 		if (lindex > 0)
// 			vec = loadmodel->vertexes[loadmodel->edges[lindex].v[0]].position;
// 		else
// 			vec = loadmodel->vertexes[loadmodel->edges[-lindex].v[1]].position;
// 		VectorCopy (vec, verts[numverts]);
// 		numverts++;
// 	}

// 	SubdividePolygon (numverts, verts[0]);
// }

//=========================================================

/*
=============
EmitWaterPolys

Does a water warp on the pre-fragmented glpoly_t chain
=============
*/
void EmitWaterPolys(msurface_t *fa, image_t *texture, float *modelMatrix, float *color)
{
    vkpoly_t *p, *bp;
    float *v;
    int i;

    typedef struct
    {
        float vertex[3];
        float texCoord[2];
    } polyvert;

    struct
    {
        float model[16];
        float color[4];
        float time;
        float scroll;
    } polyUbo;

    polyUbo.color[0] = color[0];
    polyUbo.color[1] = color[1];
    polyUbo.color[2] = color[2];
    polyUbo.color[3] = color[3];
    polyUbo.time = r_newrefdef.time;

    static polyvert verts[MAX_VERTS];

    if (fa->texinfo->flags & SURF_FLOWING)
        polyUbo.scroll = (-64 * ((r_newrefdef.time * 0.5) - (int)(r_newrefdef.time * 0.5))) / 64.f;
    else
        polyUbo.scroll = 0;

    if (modelMatrix)
    {
        memcpy(polyUbo.model, modelMatrix, sizeof(float) * 16);
    }
    else
    {
        Mat_Identity(polyUbo.model);
    }

    // QVk_BindPipeline(&vk_drawPolyWarpPipeline);

    // uint32_t uboOffset;
    // VkDescriptorSet uboDescriptorSet;
    // uint8_t *uboData = QVk_GetUniformBuffer(sizeof(polyUbo), &uboOffset, &uboDescriptorSet);
    // memcpy(uboData, &polyUbo, sizeof(polyUbo));

    // VkBuffer vbo;
    // VkDeviceSize vboOffset;
    // VkDescriptorSet descriptorSets[] = {texture->vk_texture.descriptorSet, uboDescriptorSet};
    // vkCmdPushConstants(vk_activeCmdbuffer, vk_drawPolyWarpPipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0,
    //                    sizeof(r_viewproj_matrix), r_viewproj_matrix);
    // vkCmdBindDescriptorSets(vk_activeCmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_drawPolyWarpPipeline.layout, 0,
    // 2,
    //                         descriptorSets, 1, &uboOffset);

    // for (bp = fa->polys; bp; bp = bp->next)
    // {
    //     p = bp;

    //     for (i = 0, v = p->verts[0]; i < p->numverts; i++, v += VERTEXSIZE)
    //     {
    //         verts[i].vertex[0] = v[0];
    //         verts[i].vertex[1] = v[1];
    //         verts[i].vertex[2] = v[2];
    //         verts[i].texCoord[0] = v[3] / 64.f;
    //         verts[i].texCoord[1] = v[4] / 64.f;
    //     }

    //     uint8_t *vertData = QVk_GetVertexBuffer(sizeof(polyvert) * p->numverts, &vbo, &vboOffset);
    //     memcpy(vertData, verts, sizeof(polyvert) * p->numverts);

    //     vkCmdBindVertexBuffers(vk_activeCmdbuffer, 0, 1, &vbo, &vboOffset);
    //     vkCmdBindIndexBuffer(vk_activeCmdbuffer, QVk_GetTriangleFanIbo((p->numverts - 2) * 3), 0,
    //     VK_INDEX_TYPE_UINT16); vkCmdDrawIndexed(vk_activeCmdbuffer, (p->numverts - 2) * 3, 1, 0, 0, 0);
    // }
}

//===================================================================

vec3_t skyclip[6] = {{1, 1, 0}, {1, -1, 0}, {0, -1, 1}, {0, 1, 1}, {1, 0, 1}, {-1, 0, 1}};
int c_sky;

// 1 = s, 2 = t, 3 = 2048
int st_to_vec[6][3] = {
    {3, -1, 2},  {-3, 1, 2},

    {1, 3, 2},   {-1, -3, 2},

    {-2, -1, 3}, // 0 degrees yaw, look straight up
    {2, -1, -3}  // look straight down

    //	{-1,2,3},
    //	{1,2,-3}
};

// s = [0]/[2], t = [1]/[2]
int vec_to_st[6][3] = {
    {-2, 3, 1},
    {2, 3, -1},

    {1, 3, 2},
    {-1, 3, -2},

    {-2, -1, 3},
    {-2, 1, -3}

    //	{-1,2,3},
    //	{1,2,-3}
};

float skymins[2][6], skymaxs[2][6];
float sky_min, sky_max;

void DrawSkyPolygon(int nump, vec3_t vecs)
{
    int i, j;
    vec3_t v, av;
    float s, t, dv;
    int axis;
    float *vp;

    c_sky++;

    // decide which face it maps to
    VectorCopy(vec3_origin, v);
    for (i = 0, vp = vecs; i < nump; i++, vp += 3)
    {
        VectorAdd(vp, v, v);
    }
    av[0] = fabs(v[0]);
    av[1] = fabs(v[1]);
    av[2] = fabs(v[2]);
    if (av[0] > av[1] && av[0] > av[2])
    {
        if (v[0] < 0)
            axis = 1;
        else
            axis = 0;
    }
    else if (av[1] > av[2] && av[1] > av[0])
    {
        if (v[1] < 0)
            axis = 3;
        else
            axis = 2;
    }
    else
    {
        if (v[2] < 0)
            axis = 5;
        else
            axis = 4;
    }

    // project new texture coords
    for (i = 0; i < nump; i++, vecs += 3)
    {
        j = vec_to_st[axis][2];
        if (j > 0)
            dv = vecs[j - 1];
        else
            dv = -vecs[-j - 1];
        if (dv < 0.001)
            continue; // don't divide by zero
        j = vec_to_st[axis][0];
        if (j < 0)
            s = -vecs[-j - 1] / dv;
        else
            s = vecs[j - 1] / dv;
        j = vec_to_st[axis][1];
        if (j < 0)
            t = -vecs[-j - 1] / dv;
        else
            t = vecs[j - 1] / dv;

        if (s < skymins[0][axis])
            skymins[0][axis] = s;
        if (t < skymins[1][axis])
            skymins[1][axis] = t;
        if (s > skymaxs[0][axis])
            skymaxs[0][axis] = s;
        if (t > skymaxs[1][axis])
            skymaxs[1][axis] = t;
    }
}

#define ON_EPSILON 0.1 // point on plane side epsilon
#define MAX_CLIP_VERTS 64
void ClipSkyPolygon(int nump, vec3_t vecs, int stage)
{
    float *norm;
    float *v;
    qboolean front, back;
    float d, e;
    float dists[MAX_CLIP_VERTS];
    int sides[MAX_CLIP_VERTS];
    vec3_t newv[2][MAX_CLIP_VERTS];
    int newc[2];
    int i, j;

    if (nump > MAX_CLIP_VERTS - 2)
    {
        LOGF(eERROR, "ClipSkyPolygon: MAX_CLIP_VERTS");
        // throw
    }
    if (stage == 6)
    { // fully clipped, so draw it
        DrawSkyPolygon(nump, vecs);
        return;
    }

    front = back = false;
    norm = skyclip[stage];
    for (i = 0, v = vecs; i < nump; i++, v += 3)
    {
        d = DotProduct(v, norm);
        if (d > ON_EPSILON)
        {
            front = true;
            sides[i] = SIDE_FRONT;
        }
        else if (d < -ON_EPSILON)
        {
            back = true;
            sides[i] = SIDE_BACK;
        }
        else
            sides[i] = SIDE_ON;
        dists[i] = d;
    }

    if (!front || !back)
    { // not clipped
        ClipSkyPolygon(nump, vecs, stage + 1);
        return;
    }

    // clip it
    sides[i] = sides[0];
    dists[i] = dists[0];
    VectorCopy(vecs, (vecs + (i * 3)));
    newc[0] = newc[1] = 0;

    for (i = 0, v = vecs; i < nump; i++, v += 3)
    {
        switch (sides[i])
        {
        case SIDE_FRONT:
            VectorCopy(v, newv[0][newc[0]]);
            newc[0]++;
            break;
        case SIDE_BACK:
            VectorCopy(v, newv[1][newc[1]]);
            newc[1]++;
            break;
        case SIDE_ON:
            VectorCopy(v, newv[0][newc[0]]);
            newc[0]++;
            VectorCopy(v, newv[1][newc[1]]);
            newc[1]++;
            break;
        }

        if (sides[i] == SIDE_ON || sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i])
            continue;

        d = dists[i] / (dists[i] - dists[i + 1]);
        for (j = 0; j < 3; j++)
        {
            e = v[j] + d * (v[j + 3] - v[j]);
            newv[0][newc[0]][j] = e;
            newv[1][newc[1]][j] = e;
        }
        newc[0]++;
        newc[1]++;
    }

    // continue
    ClipSkyPolygon(newc[0], newv[0][0], stage + 1);
    ClipSkyPolygon(newc[1], newv[1][0], stage + 1);
}

/*
=================
R_AddSkySurface
=================
*/
void R_AddSkySurface(msurface_t *fa)
{
    int i;
    vec3_t verts[MAX_CLIP_VERTS];
    vkpoly_t *p;

    // calculate vertex values for sky box
    for (p = fa->polys; p; p = p->next)
    {
        for (i = 0; i < p->numverts; i++)
        {
            VectorSubtract(p->verts[i], r_origin, verts[i]);
        }
        ClipSkyPolygon(p->numverts, verts[0], 0);
    }
}

/*
==============
R_ClearSkyBox
==============
*/
void R_ClearSkyBox(void)
{
    int i;

    for (i = 0; i < 6; i++)
    {
        skymins[0][i] = skymins[1][i] = 9999;
        skymaxs[0][i] = skymaxs[1][i] = -9999;
    }
}

void MakeSkyVec(float s, float t, int axis, float *vertexData)
{
    vec3_t v, b;
    int j, k;

    b[0] = s * 2300;
    b[1] = t * 2300;
    b[2] = 2300;

    for (j = 0; j < 3; j++)
    {
        k = st_to_vec[axis][j];
        if (k < 0)
            v[j] = -b[-k - 1];
        else
            v[j] = b[k - 1];
    }

    // avoid bilerp seam
    s = (s + 1) * 0.5;
    t = (t + 1) * 0.5;

    if (s < sky_min)
        s = sky_min;
    else if (s > sky_max)
        s = sky_max;
    if (t < sky_min)
        t = sky_min;
    else if (t > sky_max)
        t = sky_max;

    t = 1.0 - t;

    vertexData[0] = v[0];
    vertexData[1] = v[1];
    vertexData[2] = v[2];
    vertexData[3] = s;
    vertexData[4] = t;
}

/*
==============
R_DrawSkyBox
==============
*/
int skytexorder[6] = {0, 2, 1, 3, 4, 5};
void R_DrawSkyBox(void)
{
    int i;

    if (skyrotate)
    { // check for no sky at all
        for (i = 0; i < 6; i++)
            if (skymins[0][i] < skymaxs[0][i] && skymins[1][i] < skymaxs[1][i])
                break;
        if (i == 6)
            return; // nothing visible
    }

    float model[16];
    Mat_Identity(model);
    Mat_Rotate(model, r_newrefdef.time * skyrotate, skyaxis[0], skyaxis[1], skyaxis[2]);
    Mat_Translate(model, r_origin[0], r_origin[1], r_origin[2]);

    struct
    {
        float data[5];
    } skyVerts[4];

    cmdBindPipeline(pCmd, drawSkyboxPipeline);
    GPURingBufferOffset uniformBlock = getGPURingBufferOffset(&dynamicUniformBuffer, sizeof(model));
    BufferUpdateDesc updateDesc = {uniformBlock.pBuffer, uniformBlock.mOffset};

    beginUpdateResource(&updateDesc);
    memcpy(updateDesc.pMappedData, model, sizeof(model));
    endUpdateResource(&updateDesc);

    for (i = 0; i < 6; i++)
    {
        if (skyrotate)
        { // hack, forces full sky to draw when rotating
            skymins[0][i] = -1;
            skymins[1][i] = -1;
            skymaxs[0][i] = 1;
            skymaxs[1][i] = 1;
        }

        if (skymins[0][i] >= skymaxs[0][i] || skymins[1][i] >= skymaxs[1][i])
            continue;

        MakeSkyVec(skymins[0][i], skymins[1][i], i, skyVerts[0].data);
        MakeSkyVec(skymins[0][i], skymaxs[1][i], i, skyVerts[1].data);
        MakeSkyVec(skymaxs[0][i], skymaxs[1][i], i, skyVerts[2].data);
        MakeSkyVec(skymaxs[0][i], skymins[1][i], i, skyVerts[3].data);

        float verts[] = {
            skyVerts[0].data[0], skyVerts[0].data[1], skyVerts[0].data[2], skyVerts[0].data[3], skyVerts[0].data[4],
            skyVerts[1].data[0], skyVerts[1].data[1], skyVerts[1].data[2], skyVerts[1].data[3], skyVerts[1].data[4],
            skyVerts[2].data[0], skyVerts[2].data[1], skyVerts[2].data[2], skyVerts[2].data[3], skyVerts[2].data[4],
            skyVerts[0].data[0], skyVerts[0].data[1], skyVerts[0].data[2], skyVerts[0].data[3], skyVerts[0].data[4],
            skyVerts[2].data[0], skyVerts[2].data[1], skyVerts[2].data[2], skyVerts[2].data[3], skyVerts[2].data[4],
            skyVerts[3].data[0], skyVerts[3].data[1], skyVerts[3].data[2], skyVerts[3].data[3], skyVerts[3].data[4]};

        GPURingBufferOffset vertexBuffer = getGPURingBufferOffset(&dynamicVertexBuffer, sizeof(model));
        BufferUpdateDesc updateDesc = {vertexBuffer.pBuffer, vertexBuffer.mOffset};

        beginUpdateResource(&updateDesc);
        memcpy(updateDesc.pMappedData, verts, sizeof(verts));
        endUpdateResource(&updateDesc);

        cmdBindPushConstants(pCmd, pRootSignature, gPushConstant, r_viewproj_matrix);
        cmdBindDescriptorSet(pCmd, 0, pDescriptorSetsTexture[sky_images[skytexorder[i]]->index]);

        DescriptorDataRange range = {(uint32_t)uniformBlock.mOffset, sizeof(model)};
        DescriptorData params[1] = {};
        params[0].pName = "UniformBufferObject_rootcbv";
        params[0].ppBuffers = &uniformBlock.pBuffer;
        params[0].pRanges = &range;

        cmdBindDescriptorSetWithRootCbvs(pCmd, 0, pDescriptorSetUniforms, 1, params);

        constexpr uint32_t stride = sizeof(float) * 5;
        cmdBindVertexBuffer(pCmd, 1, &vertexBuffer.pBuffer, &stride, &vertexBuffer.mOffset);

		cmdDraw(pCmd, 6, 0);
    }
}

/*
============
R_SetSky
============
*/
// 3dstudio environment map names
const char *suf[6] = {"rt", "bk", "lf", "ft", "up", "dn"};
void R_SetSky(char *name, float rotate, vec3_t axis)
{
    LOGF(eINFO, "Set sky: %s", name);

    int i;
    char pathname[MAX_QPATH];

    strncpy(skyname, name, sizeof(skyname) - 1);
    skyrotate = rotate;
    VectorCopy(axis, skyaxis);

    for (i = 0; i < 6; i++)
    {
        // chop down rotating skies for less memory
        // if (vk_skymip->value || skyrotate)
        //     vk_picmip->value++;

        std::string pathname = std::format("env/{}{}.tga", skyname, suf[i]);

        sky_images[i] = GRA_FindImage(pathname, it_sky);
        if (!sky_images[i])
        {
            sky_images[i] = r_notexture;
        }

        // if (vk_skymip->value || skyrotate)
        // { // take less memory
        //     vk_picmip->value--;
        //     sky_min = 1.0 / 256;
        //     sky_max = 255.0 / 256;
        // }
        // else
        {
            sky_min = 1.0 / 512;
            sky_max = 511.0 / 512;
        }
    }
}
