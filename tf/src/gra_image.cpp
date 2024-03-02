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

#include "gra_local.h"

#include <ILog.h>
#include <IResourceLoader.h>
#include <memory>

std::map<std::string, image_t> textures;

// texture for storing raw image data (cinematics, endscreens, etc.)
Texture *rawTexture;

static byte intensitytable[256];
static unsigned char gammatable[256];

cvar_t *intensity;
extern cvar_t *vk_mip_nearfilter;

unsigned d_8to24table[256];

Sampler *pSamplerImage = NULL;
static Sampler *pSamplerLightmap = NULL;

extern Renderer *pRenderer;

void GRA_CreateTexture(Texture *&texture, const std::string &name, const unsigned char *data, uint32_t width,
                       uint32_t height)
{
    SyncToken token = {};
    TextureDesc textureDesc = {
        .pName = name.c_str(),
        .mWidth = width,
        .mHeight = height,
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
    loadDesc.ppTexture = &texture;

    addResource(&loadDesc, &token);
    waitForToken(&token);

    TextureUpdateDesc updateDesc{
        .pTexture = texture,
        .mBaseMipLevel = 0,
        .mMipLevels = 1,
        .mBaseArrayLayer = 0,
        .mLayerCount = 1,
        .mCurrentState = RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
    };

    beginUpdateResource(&updateDesc);

    TextureSubresourceUpdate subresource = updateDesc.getSubresourceUpdateDesc(0, 0);
    for (uint32_t r = 0; r < subresource.mRowCount; ++r)
    {
        memcpy(subresource.pMappedData + r * subresource.mDstRowStride, data + r * subresource.mSrcRowStride,
               subresource.mSrcRowStride);
    }
    endUpdateResource(&updateDesc);
}

void GRA_ImageList_f(void)
{
    int texels = 0;

    LOGF(LogLevel::eINFO, "------------------");

    for (auto &texturePair : textures)
    {
        auto &[name, image] = texturePair;
        if (image.texture == NULL)
        {
            continue;
        }

        texels += image.width * image.height;

        std::string typeStr;
        switch (image.type)
        {
        case it_skin:
            typeStr = "M";
            break;
        case it_sprite:
            typeStr = "S";
            break;
        case it_wall:
            typeStr = "W";
            break;
        case it_pic:
            typeStr = "P";
            break;
        default:
            typeStr = " ";
            break;
        }

        LOGF(LogLevel::eINFO, " %3i %3i RGB: %s\n", image.width, image.height, name);
    }
    LOGF(LogLevel::eINFO, "Total texel count (not counting mipmaps): %i\n", texels);
}

/*
==============
LoadPCX
==============
*/
void LoadPCX(std::string filename, byte **pic, byte **palette, int *width, int *height)
{
    byte *raw;
    pcx_t *pcx;
    int x, y;
    int len;
    int dataByte, runLength;
    byte *out, *pix;

    *pic = NULL;
    *palette = NULL;

    //
    // load the file
    //
    len = FS_LoadFile(filename.data(), (void **)&raw);
    if (!raw)
    {
        LOGF(LogLevel::eDEBUG, "Bad pcx file %s", filename.c_str());
        return;
    }

    //
    // parse the PCX file
    //
    pcx = (pcx_t *)raw;

    raw = &pcx->data;

    if (pcx->manufacturer != 0x0a || pcx->version != 5 || pcx->encoding != 1 || pcx->bits_per_pixel != 8 ||
        pcx->xmax >= 640 || pcx->ymax >= 480)
    {
        LOGF(LogLevel::eDEBUG, "Bad pcx file %s\n", filename);
        return;
    }

    out = (byte *)tf_malloc((pcx->ymax + 1) * (pcx->xmax + 1));

    *pic = out;

    pix = out;

    if (palette)
    {
        *palette = (byte *)tf_malloc(768);
        memcpy(*palette, (byte *)pcx + len - 768, 768);
    }

    if (width)
        *width = pcx->xmax + 1;
    if (height)
        *height = pcx->ymax + 1;

    for (y = 0; y <= pcx->ymax; y++, pix += pcx->xmax + 1)
    {
        for (x = 0; x <= pcx->xmax;)
        {
            dataByte = *raw++;

            if ((dataByte & 0xC0) == 0xC0)
            {
                runLength = dataByte & 0x3F;
                dataByte = *raw++;
            }
            else
                runLength = 1;

            while (runLength-- > 0)
                pix[x++] = dataByte;
        }
    }

    if (raw - (byte *)pcx > len)
    {
        LOGF(eDEBUG, "PCX file %s was malformed", filename);
        tf_free(*pic);
        *pic = NULL;
    }

    FS_FreeFile(pcx);
}

/*
=========================================================

TARGA LOADING

=========================================================
*/

typedef struct _TargaHeader
{
    unsigned char id_length, colormap_type, image_type;
    unsigned short colormap_index, colormap_length;
    unsigned char colormap_size;
    unsigned short x_origin, y_origin, width, height;
    unsigned char pixel_size, attributes;
} TargaHeader;

static void LoadTGA(char *name, byte **pic, int *width, int *height)
{
    int columns, rows, numPixels;
    byte *pixbuf;
    int row, column;
    byte *buf_p;
    byte *buffer;
    int length;
    TargaHeader targa_header;
    byte *targa_rgba;
    byte tmp[2];

    *pic = NULL;

    //
    // load the file
    //
    length = FS_LoadFile(name, (void **)&buffer);
    if (!buffer)
    {
        LOGF(LogLevel::eDEBUG, "Bad tga file %s", name);
        return;
    }

    buf_p = buffer;

    targa_header.id_length = *buf_p++;
    targa_header.colormap_type = *buf_p++;
    targa_header.image_type = *buf_p++;

    tmp[0] = buf_p[0];
    tmp[1] = buf_p[1];
    targa_header.colormap_index = LittleShort(*((short *)tmp));
    buf_p += 2;
    tmp[0] = buf_p[0];
    tmp[1] = buf_p[1];
    targa_header.colormap_length = LittleShort(*((short *)tmp));
    buf_p += 2;
    targa_header.colormap_size = *buf_p++;
    targa_header.x_origin = LittleShort(*((short *)buf_p));
    buf_p += 2;
    targa_header.y_origin = LittleShort(*((short *)buf_p));
    buf_p += 2;
    targa_header.width = LittleShort(*((short *)buf_p));
    buf_p += 2;
    targa_header.height = LittleShort(*((short *)buf_p));
    buf_p += 2;
    targa_header.pixel_size = *buf_p++;
    targa_header.attributes = *buf_p++;

    if (targa_header.image_type != 2 && targa_header.image_type != 10)
    {
        LOGF(LogLevel::eERROR, "LoadTGA: Only type 2 and 10 targa RGB images supported.");
    }
    if (targa_header.colormap_type != 0 || (targa_header.pixel_size != 32 && targa_header.pixel_size != 24))
    {
        LOGF(LogLevel::eERROR, "LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");
    }

    columns = targa_header.width;
    rows = targa_header.height;
    numPixels = columns * rows;

    if (width)
        *width = columns;
    if (height)
        *height = rows;

    targa_rgba = (byte *)tf_malloc(numPixels * 4);
    *pic = targa_rgba;

    if (targa_header.id_length != 0)
        buf_p += targa_header.id_length; // skip TARGA image comment

    if (targa_header.image_type == 2)
    { // Uncompressed, RGB images
        for (row = rows - 1; row >= 0; row--)
        {
            pixbuf = targa_rgba + row * columns * 4;
            for (column = 0; column < columns; column++)
            {
                unsigned char red, green, blue, alphabyte;
                switch (targa_header.pixel_size)
                {
                case 24:

                    blue = *buf_p++;
                    green = *buf_p++;
                    red = *buf_p++;
                    *pixbuf++ = red;
                    *pixbuf++ = green;
                    *pixbuf++ = blue;
                    *pixbuf++ = 255;
                    break;
                case 32:
                    blue = *buf_p++;
                    green = *buf_p++;
                    red = *buf_p++;
                    alphabyte = *buf_p++;
                    *pixbuf++ = red;
                    *pixbuf++ = green;
                    *pixbuf++ = blue;
                    *pixbuf++ = alphabyte;
                    break;
                }
            }
        }
    }
    else if (targa_header.image_type == 10)
    { // Runlength encoded RGB images
        unsigned char red = 0, green = 0, blue = 0, alphabyte = 0, packetHeader, packetSize, j;
        for (row = rows - 1; row >= 0; row--)
        {
            pixbuf = targa_rgba + row * columns * 4;
            for (column = 0; column < columns;)
            {
                packetHeader = *buf_p++;
                packetSize = 1 + (packetHeader & 0x7f);
                if (packetHeader & 0x80)
                { // run-length packet
                    switch (targa_header.pixel_size)
                    {
                    case 24:
                        blue = *buf_p++;
                        green = *buf_p++;
                        red = *buf_p++;
                        alphabyte = 255;
                        break;
                    case 32:
                        blue = *buf_p++;
                        green = *buf_p++;
                        red = *buf_p++;
                        alphabyte = *buf_p++;
                        break;
                    }

                    for (j = 0; j < packetSize; j++)
                    {
                        *pixbuf++ = red;
                        *pixbuf++ = green;
                        *pixbuf++ = blue;
                        *pixbuf++ = alphabyte;
                        column++;
                        if (column == columns)
                        { // run spans across rows
                            column = 0;
                            if (row > 0)
                                row--;
                            else
                                goto breakOut;
                            pixbuf = targa_rgba + row * columns * 4;
                        }
                    }
                }
                else
                { // non run-length packet
                    for (j = 0; j < packetSize; j++)
                    {
                        switch (targa_header.pixel_size)
                        {
                        case 24:
                            blue = *buf_p++;
                            green = *buf_p++;
                            red = *buf_p++;
                            *pixbuf++ = red;
                            *pixbuf++ = green;
                            *pixbuf++ = blue;
                            *pixbuf++ = 255;
                            break;
                        case 32:
                            blue = *buf_p++;
                            green = *buf_p++;
                            red = *buf_p++;
                            alphabyte = *buf_p++;
                            *pixbuf++ = red;
                            *pixbuf++ = green;
                            *pixbuf++ = blue;
                            *pixbuf++ = alphabyte;
                            break;
                        }
                        column++;
                        if (column == columns)
                        { // pixel packet run spans across rows
                            column = 0;
                            if (row > 0)
                                row--;
                            else
                                goto breakOut;
                            pixbuf = targa_rgba + row * columns * 4;
                        }
                    }
                }
            }
        breakOut:;
        }
    }

    FS_FreeFile(buffer);
}

/*
====================================================================

IMAGE FLOOD FILLING

====================================================================
*/

/*
=================
Mod_FloodFillSkin

Fill background pixels so mipmapping doesn't have haloes
=================
*/

typedef struct
{
    short x, y;
} floodfill_t;

// must be a power of 2
#define FLOODFILL_FIFO_SIZE 0x1000
#define FLOODFILL_FIFO_MASK (FLOODFILL_FIFO_SIZE - 1)

#define FLOODFILL_STEP(off, dx, dy)                                                                                    \
    {                                                                                                                  \
        if (pos[off] == fillcolor)                                                                                     \
        {                                                                                                              \
            pos[off] = 255;                                                                                            \
            fifo[inpt].x = x + (dx), fifo[inpt].y = y + (dy);                                                          \
            inpt = (inpt + 1) & FLOODFILL_FIFO_MASK;                                                                   \
        }                                                                                                              \
        else if (pos[off] != 255)                                                                                      \
            fdc = pos[off];                                                                                            \
    }

void R_FloodFillSkin(byte *skin, int skinwidth, int skinheight)
{
    byte fillcolor = *skin; // assume this is the pixel to fill
    floodfill_t fifo[FLOODFILL_FIFO_SIZE];
    int inpt = 0, outpt = 0;
    int filledcolor = -1;
    int i;

    if (filledcolor == -1)
    {
        filledcolor = 0;
        // attempt to find opaque black
        for (i = 0; i < 256; ++i)
            if (d_8to24table[i] == (255 << 0)) // alpha 1.0
            {
                filledcolor = i;
                break;
            }
    }

    // can't fill to filled color or to transparent color (used as visited marker)
    if ((fillcolor == filledcolor) || (fillcolor == 255))
    {
        // printf( "not filling skin from %d to %d\n", fillcolor, filledcolor );
        return;
    }

    fifo[inpt].x = 0, fifo[inpt].y = 0;
    inpt = (inpt + 1) & FLOODFILL_FIFO_MASK;

    while (outpt != inpt)
    {
        int x = fifo[outpt].x, y = fifo[outpt].y;
        int fdc = filledcolor;
        byte *pos = &skin[x + skinwidth * y];

        outpt = (outpt + 1) & FLOODFILL_FIFO_MASK;

        if (x > 0)
            FLOODFILL_STEP(-1, -1, 0);
        if (x < skinwidth - 1)
            FLOODFILL_STEP(1, 1, 0);
        if (y > 0)
            FLOODFILL_STEP(-skinwidth, 0, -1);
        if (y < skinheight - 1)
            FLOODFILL_STEP(skinwidth, 0, 1);
        skin[x + skinwidth * y] = fdc;
    }
}

byte *GRA_MapPalleteImage(byte *data, int width, int height)
{
    int size = width * height;
    uint32_t *output = reinterpret_cast<uint32_t *>(tf_malloc(size * sizeof(uint32_t)));

    for (int i = 0; i < size; i++)
    {
        auto p = data[i];
        output[i] = d_8to24table[p];

        if (p == 255)
        { // transparent, so scan around for another color
            // to avoid alpha fringes
            // FIXME: do a full flood fill so mips work...
            if (i > width && data[i - width] != 255)
                p = data[i - width];
            else if (i < size - width && data[i + width] != 255)
                p = data[i + width];
            else if (i > 0 && data[i - 1] != 255)
                p = data[i - 1];
            else if (i < size - 1 && data[i + 1] != 255)
                p = data[i + 1];
            else
                p = 0;
            // copy rgb components
            ((byte *)&output[i])[0] = ((byte *)&d_8to24table[p])[0];
            ((byte *)&output[i])[1] = ((byte *)&d_8to24table[p])[1];
            ((byte *)&output[i])[2] = ((byte *)&d_8to24table[p])[2];
        }
    }

    return reinterpret_cast<byte *>(output);
}

image_t *GRA_LoadPic(const std::string &name, byte *pic, int width, int height, imagetype_t type, int bits)
{
    if (textures.contains(name))
    {
        return &textures.at(name);
    }

    image_t image = {
        .name = name,
        .type = type,
        .width = width,
        .height = height,
        .registration_sequence = registration_sequence,
    };

    if (pic == NULL)
    {
        return NULL;
    }

    if (type == it_skin && bits == 8)
    {
        R_FloodFillSkin(pic, width, height);
    }

    if (bits == 8)
    {
        auto mappedData = GRA_MapPalleteImage(pic, width, height);

        if (mappedData == NULL)
        {
            return NULL;
        }

        GRA_CreateTexture(image.texture, image.name, (unsigned char *)mappedData, image.width, image.height);
        tf_free(mappedData);
    }
    else
    {
        GRA_CreateTexture(image.texture, image.name, (unsigned char *)pic, image.width, image.height);
    }

    textures.emplace(name, image);

    return &textures.at(name);
}

image_t *GRA_LoadWal(char *name)
{
    miptex_t *mt;
    int width, height, ofs;
    image_t *image;

    FS_LoadFile(name, (void **)&mt);
    if (!mt)
    {
        LOGF(eERROR, "GRA_FindImage: can't load %s\n", name);
        return r_notexture;
    }

    width = LittleLong(mt->width);
    height = LittleLong(mt->height);
    ofs = LittleLong(mt->offsets[0]);

    image = GRA_LoadPic(name, (byte *)mt + ofs, width, height, it_wall, 8);

    FS_FreeFile((void *)mt);

    return image;
}

image_t *GRA_FindImage(std::string name, imagetype_t type)
{
    int i, len;
    byte *pic, *palette;
    int width, height;
    image_t *image;

    if (textures.contains(name))
    {
        image = &textures.at(name);
        image->registration_sequence = registration_sequence;

        return image;
    }

    //
    // load the pic from disk
    //
    pic = NULL;
    palette = NULL;

    auto ext = name.substr(name.length() - 4);
    if (ext == ".pcx")
    {
        LoadPCX(name.data(), &pic, &palette, &width, &height);
        if (!pic)
        {
            return NULL; // ri.Sys_Error (ERR_DROP, "GRA_FindImage: can't load %s", name);
        }
        image = GRA_LoadPic(name, pic, width, height, type, 8);
    }
    else if (ext == ".wal")
    {
        image = GRA_LoadWal(name.data());
    }
    else if (ext == ".tga")
    {
        LoadTGA(name.data(), &pic, &width, &height);
        if (!pic)
        {
            return NULL; // ri.Sys_Error (ERR_DROP, "GRA_FindImage: can't load %s", name);
        }
        image = GRA_LoadPic(name, pic, width, height, type, 32);
    }
    else
    {
        return NULL; //	ri.Sys_Error (ERR_DROP, "GRA_FindImage: bad extension on: %s", name);
    }

    if (pic)
    {
        tf_free(pic);
    }
    if (palette)
    {
        tf_free(palette);
    }

    return image;
}

struct image_s *R_RegisterSkin(char *name)
{
    LOGF(eINFO, "Register skin: %s.", name);

    return GRA_FindImage(name, it_skin);
}

void GRA_FreeUnusedImages(void)
{
    int i;
    image_t *image;

    // never free r_notexture or particle texture
    r_notexture->registration_sequence = registration_sequence;
    r_particletexture->registration_sequence = registration_sequence;

    for (auto &pair : textures)
    {
        auto &[name, image] = pair;
        if (image.registration_sequence == registration_sequence)
            continue; // used this sequence

        if (image.type == it_pic)
            continue; // don't free pics
        // free it
        removeResource(image.texture);
        textures.erase(name);
    }
}

int Draw_LoadPalette(void)
{
    int i;
    int r, g, b;
    unsigned v;
    byte *pic, *pal;
    int width, height;

    // get the palette

    LoadPCX("pics/colormap.pcx", &pic, &pal, &width, &height);
    if (!pal)
    {
        LOGF(eERROR, "Couldn't load pics/colormap.pcx");
    }

    for (i = 0; i < 256; i++)
    {
        r = pal[i * 3 + 0];
        g = pal[i * 3 + 1];
        b = pal[i * 3 + 2];

        v = (255 << 24) + (r << 0) + (g << 8) + (b << 16);
        d_8to24table[i] = LittleLong(v);
    }

    d_8to24table[255] &= LittleLong(0xffffff); // 255 is transparent

    free(pic);
    free(pal);

    return 0;
}

void GRA_InitImages(void)
{
    int i, j;
    float g = vid_gamma->value;

    registration_sequence = 1;

    // init intensity conversions
    intensity = Cvar_Get(std::string("intensity").data(), std::string("2").data(), 0);

    if (intensity->value <= 1)
    {
        Cvar_Set(std::string("intensity").data(), std::string("1").data());
    }

    vk_state.inverse_intensity = 1 / intensity->value;

    Draw_LoadPalette();

    for (i = 0; i < 256; i++)
    {
        if (g == 1)
        {
            gammatable[i] = i;
        }
        else
        {
            float inf;

            inf = 255 * pow((i + 0.5) / 255.5, g) + 0.5;
            if (inf < 0)
                inf = 0;
            if (inf > 255)
                inf = 255;
            gammatable[i] = inf;
        }
    }

    for (i = 0; i < 256; i++)
    {
        j = i * intensity->value;
        if (j > 255)
            j = 255;
        intensitytable[i] = j;
    }
}

void GRA_ShutdownImages(void)
{
    for (auto &pair : textures)
    {
        auto &[name, image] = pair;
        removeResource(image.texture);
    }

    removeResource(rawTexture);

    for (int i = 0; i < MAX_LIGHTMAPS * 2; i++)
    {
        removeResource(vk_state.lightmap_textures[i]);
    }
}
