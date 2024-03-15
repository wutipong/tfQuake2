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

// draw.c

#include "../../client/vid.h"
#include "gra_common.h"
#include "gra_local.h"
#include <format>

image_t *draw_chars;

bool vk_frameStarted;

extern viddef_t vid;

/*
===============
Draw_InitLocal
===============
*/
void Draw_InitLocal(void)
{
    // load console characters (don't bilerp characters)
    // qvksampler_t samplerType = S_NEAREST;
    draw_chars = GRA_FindImage("pics/conchars.pcx", it_pic);
}

/*
================
Draw_Char

Draws one 8*8 graphics character with 0 being transparent.
It can be clipped to the top of the screen to allow the console to be
smoothly scrolled off.
================
*/
void Draw_Char(int x, int y, int num)
{
    int row, col;
    float frow, fcol, size;

    if (!vk_frameStarted)
    {
        return;
    }

    num &= 255;

    if ((num & 127) == 32)
    {
        return; // space
    }

    cvar_t *scale = Cvar_Get(std::string("hudscale").data(), std::string("1").data(), 0);

    if (y <= -8 * scale->value)
    {
        return; // totally off screen
    }
    row = num >> 4;
    col = num & 15;

    frow = row * 0.0625;
    fcol = col * 0.0625;
    size = 0.0625;

    float imgTransform[] = {(float)x / vid.width,
                            (float)y / vid.height,
                            8.f * scale->value / vid.width,
                            8.f * scale->value / vid.height,
                            fcol,
                            frow,
                            size,
                            size};
    GRA_DrawTexRect(imgTransform, sizeof(imgTransform), draw_chars);
}

/*
=============
Draw_FindPic
=============
*/
image_t *Draw_FindPic(char *name)
{
    image_t *vk;

    if (name[0] != '/' && name[0] != '\\')
    {
        vk = GRA_FindImage(std::format("pics/{}.pcx", name), it_pic);
    }
    else
    {
        vk = GRA_FindImage(name + 1, it_pic);
    }

    return vk;
}

/*
=============
Draw_GetPicSize
=============
*/
void Draw_GetPicSize(int *w, int *h, char *pic)
{
    image_t *vk;

    vk = Draw_FindPic(pic);
    if (!vk)
    {
        *w = *h = -1;
        return;
    }

    cvar_t *scale = Cvar_Get(std::string("hudscale").data(), std::string("1").data(), 0);

    *w = vk->width * scale->value;
    *h = vk->height * scale->value;
}

/*
=============
Draw_StretchPic
=============
*/
void Draw_StretchPic(int x, int y, int w, int h, char *pic)
{
    image_t *vk;

    if (!vk_frameStarted)
        return;

    vk = Draw_FindPic(pic);
    if (!vk)
    {
        LOGF(eERROR, "Can't find pic: %s", pic);
        return;
    }

    // FIXME (ww) uv scale-offset might need correction.
    float imgTransform[] = {
        (float)x / vid.width, (float)y / vid.height, (float)w / vid.width, (float)h / vid.height, 0, 0, 1, 1,
    };
    GRA_DrawTexRect(imgTransform, sizeof(imgTransform), vk);
}

/*
=============
Draw_Pic
=============
*/
void Draw_Pic(int x, int y, char *pic)
{
    image_t *vk;
    cvar_t *scale = Cvar_Get(std::string("hudscale").data(), std::string("1").data(), 0);

    vk = Draw_FindPic(pic);
    if (!vk)
    {
        LOGF(eERROR, "Can't find pic: %s\n", pic);
        return;
    }

    Draw_StretchPic(x, y, vk->width * scale->value, vk->height * scale->value, pic);
}

/*
=============
Draw_TileClear

This repeats a 64*64 tile graphic to fill the screen around a sized down
refresh window.
=============
*/
void Draw_TileClear(int x, int y, int w, int h, char *pic)
{
    image_t *image;

    image = Draw_FindPic(pic);
    if (!image)
    {
        LOGF(eERROR, "Can't find pic: %s\n", pic);
        return;
    }

    float imgTransform[] = {(float)x / vid.width, (float)y / vid.height, (float)w / vid.width, (float)h / vid.height,
                            (float)x / 64.0f,     (float)y / 64.0f,      (float)w / 64.0f,     (float)h / 64.0f};
    GRA_DrawTexRect(imgTransform, sizeof(imgTransform), image);
}

/*
=============
Draw_Fill

Fills a box of pixels with a single color
=============
*/
void Draw_Fill(int x, int y, int w, int h, int c)
{
    union {
        unsigned c;
        byte v[4];
    } color;

    if (!vk_frameStarted)
        return;

    if ((unsigned)c > 255)
    {
        LOGF(eERROR, "Draw_Fill: bad color");
    }

    color.c = d_8to24table[c];

    float imgTransform[] = {(float)x / vid.width, (float)y / vid.height, (float)w / vid.width, (float)h / vid.height,
                            color.v[0] / 255.f,   color.v[1] / 255.f,    color.v[2] / 255.f,   1.f};
    GRA_DrawColorRect(imgTransform, sizeof(imgTransform), RenderPass::UI);
}

//=============================================================================

/*
================
Draw_FadeScreen

================
*/
void Draw_FadeScreen(void)
{
    float imgTransform[] = {0.f, 0.f, static_cast<float>(vid.width), static_cast<float>(vid.height), 0.f, 0.f,
                            0.f, .8f};

    if (!vk_frameStarted)
    {
        return;
    }

    GRA_DrawColorRect(imgTransform, sizeof(imgTransform), RenderPass::UI);
}

//====================================================================

/*
=============
Draw_StretchRaw
=============
*/
extern unsigned r_rawpalette[256];
extern Texture *rawTexture;

void Draw_StretchRaw(int x, int y, int w, int h, int cols, int rows, byte *data)
{
    unsigned image32[256 * 256];
    int i, j, trows;
    byte *source;
    int frac, fracstep;
    float hscale;
    int row;
    float t;

    if (!vk_frameStarted)
    {
        return;
    }

    if (rows <= 256)
    {
        hscale = 1;
        trows = rows;
    }
    else
    {
        hscale = rows / 256.0;
        trows = 256;
    }
    t = rows * hscale / 256;

    unsigned *dest;

    for (i = 0; i < trows; i++)
    {
        row = (int)(i * hscale);
        if (row > rows)
            break;
        source = data + cols * row;
        dest = &image32[i * 256];
        fracstep = cols * 0x10000 / 256;
        frac = fracstep >> 1;
        for (j = 0; j < 256; j++)
        {
            dest[j] = r_rawpalette[source[frac >> 16]];
            frac += fracstep;
        }
    }

    // if (vk_rawTexture.image != VK_NULL_HANDLE)
    // {
    //     QVk_UpdateTextureData(&vk_rawTexture, (unsigned char *)&image32, 0, 0, 256, 256);
    // }
    // else
    // {
    //     QVVKTEXTURE_CLEAR(vk_rawTexture);
    //     QVk_CreateTexture(&vk_rawTexture, (unsigned char *)&image32, 256, 256, vk_current_sampler);
    //     QVk_DebugSetObjectName((uint64_t)vk_rawTexture.image, VK_OBJECT_TYPE_IMAGE, "Image: raw texture");
    //     QVk_DebugSetObjectName((uint64_t)vk_rawTexture.imageView, VK_OBJECT_TYPE_IMAGE_VIEW, "Image View: raw
    //     texture"); QVk_DebugSetObjectName((uint64_t)vk_rawTexture.descriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET,
    //                            "Descriptor Set: raw texture");
    //     QVk_DebugSetObjectName((uint64_t)vk_rawTexture.allocInfo.deviceMemory, VK_OBJECT_TYPE_DEVICE_MEMORY,
    //                            "Memory: raw texture");
    // }

    // float imgTransform[] = {
    //     (float)x / vid.width, (float)y / vid.height, (float)w / vid.width, (float)h / vid.height, 0.f, 0.f, 1.f, t};
    // QVk_DrawTexRect(imgTransform, sizeof(imgTransform), &vk_rawTexture);
}
