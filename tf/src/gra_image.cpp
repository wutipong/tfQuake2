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

int base_textureid; // gltextures[i] = base_textureid+i
// texture for storing raw image data (cinematics, endscreens, etc.)
// qvktexture_t vk_rawTexture = QVVKTEXTURE_INIT;

static byte intensitytable[256];
static unsigned char gammatable[256];

cvar_t *intensity;
extern cvar_t *vk_mip_nearfilter;

unsigned d_8to24table[256];

// uint32_t Vk_Upload8(byte *data, int width, int height, qboolean mipmap, qboolean is_sky);
// uint32_t Vk_Upload32(unsigned *data, int width, int height, qboolean mipmap);

// default global texture and lightmap samplers
// qvksampler_t vk_current_sampler = S_MIPMAP_LINEAR;
// qvksampler_t vk_current_lmap_sampler = S_MIPMAP_LINEAR;

Sampler *pSamplerImage = NULL;
static Sampler *pSamplerLightmap = NULL;

extern Renderer *pRenderer;

void GRA_CreateTexture(Texture *&texture, const std::string &name, const unsigned char *data, uint32_t width,
                       uint32_t height);

// internal helper
// static VkImageAspectFlags getDepthStencilAspect(VkFormat depthFormat)
// {
//     switch (depthFormat)
//     {
//     case VK_FORMAT_D32_SFLOAT_S8_UINT:
//     case VK_FORMAT_D24_UNORM_S8_UINT:
//     case VK_FORMAT_D16_UNORM_S8_UINT:
//         return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
//     default:
//         return VK_IMAGE_ASPECT_DEPTH_BIT;
//     }
// }

// internal helper
// static void transitionImageLayout(const VkCommandBuffer *cmdBuffer, const VkQueue *queue, const qvktexture_t
// *texture,
//                                   const VkImageLayout oldLayout, const VkImageLayout newLayout)
// {
//     VkPipelineStageFlags srcStage = 0;
//     VkPipelineStageFlags dstStage = 0;

//     VkImageMemoryBarrier imgBarrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//                                        .pNext = NULL,
//                                        .oldLayout = oldLayout,
//                                        .newLayout = newLayout,
//                                        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//                                        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//                                        .image = texture->image,
//                                        .subresourceRange.baseMipLevel = 0, // no mip mapping levels
//                                        .subresourceRange.baseArrayLayer = 0,
//                                        .subresourceRange.layerCount = 1,
//                                        .subresourceRange.levelCount = texture->mipLevels};

//     if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
//     {
//         imgBarrier.subresourceRange.aspectMask = getDepthStencilAspect(texture->format);
//     }
//     else
//     {
//         imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//     }

//     if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
//     {
//         imgBarrier.srcAccessMask = 0;
//         imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//         srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//         dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//     }
//     // transiton that may occur when updating existing image
//     else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout ==
//     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
//     {
//         imgBarrier.srcAccessMask = 0;
//         imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//         srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//         dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//     }
//     else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout ==
//     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//     {
//         if (vk_device.transferQueue == vk_device.gfxQueue)
//         {
//             imgBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//             imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//             srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//             dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//         }
//         else
//         {
//             if (vk_device.transferQueue == *queue)
//             {
//                 // if the image is exclusively shared, start queue ownership transfer process (release) - only for
//                 // VK_SHARING_MODE_EXCLUSIVE
//                 imgBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//                 imgBarrier.dstAccessMask = 0;
//                 imgBarrier.srcQueueFamilyIndex = vk_device.transferFamilyIndex;
//                 imgBarrier.dstQueueFamilyIndex = vk_device.gfxFamilyIndex;
//                 srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//                 dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//             }
//             else
//             {
//                 // continuing queue transfer (acquisition) - this will only happen for VK_SHARING_MODE_EXCLUSIVE
//                 images if (texture->sharingMode == VK_SHARING_MODE_EXCLUSIVE)
//                 {
//                     imgBarrier.srcAccessMask = 0;
//                     imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//                     imgBarrier.srcQueueFamilyIndex = vk_device.transferFamilyIndex;
//                     imgBarrier.dstQueueFamilyIndex = vk_device.gfxFamilyIndex;
//                     srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//                     dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//                 }
//                 else
//                 {
//                     imgBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//                     imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//                     srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//                     dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//                 }
//             }
//         }
//     }
//     else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
//     {
//         imgBarrier.srcAccessMask = 0;
//         imgBarrier.dstAccessMask =
//             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//         srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//         dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//     }
//     else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
//     {
//         imgBarrier.srcAccessMask = 0;
//         imgBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//         srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//         dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//     }
//     else
//     {
//         assert(0 && !"Invalid image stage!");
//     }

//     vkCmdPipelineBarrier(*cmdBuffer, srcStage, dstStage, 0, 0, NULL, 0, NULL, 1, &imgBarrier);
// }

// internal helper
// static void generateMipmaps(const VkCommandBuffer *cmdBuffer, const qvktexture_t *texture, uint32_t width,
//                             uint32_t height)
// {
//     int32_t mipWidth = width;
//     int32_t mipHeight = height;
//     VkFilter mipFilter = vk_mip_nearfilter->value > 0 ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;

//     VkImageMemoryBarrier imgBarrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//                                        .pNext = NULL,
//                                        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//                                        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//                                        .image = texture->image,
//                                        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//                                        .subresourceRange.levelCount = 1,
//                                        .subresourceRange.baseArrayLayer = 0,
//                                        .subresourceRange.layerCount = 1};

//     // copy rescaled mip data between consecutive levels (each higher level is half the size of the previous level)
//     for (uint32_t i = 1; i < texture->mipLevels; ++i)
//     {
//         imgBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//         imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//         imgBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//         imgBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//         imgBarrier.subresourceRange.baseMipLevel = i - 1;

//         vkCmdPipelineBarrier(*cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL,
//         0,
//                              NULL, 1, &imgBarrier);

//         VkImageBlit blit = {
//             .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//             .srcSubresource.mipLevel = i - 1,
//             .srcSubresource.baseArrayLayer = 0,
//             .srcSubresource.layerCount = 1,
//             .srcOffsets[0] = {0, 0, 0},
//             .srcOffsets[1] = {mipWidth, mipHeight, 1},
//             .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//             .dstSubresource.mipLevel = i,
//             .dstSubresource.baseArrayLayer = 0,
//             .dstSubresource.layerCount = 1,
//             .dstOffsets[0] = {0, 0, 0},
//             .dstOffsets[1] = {mipWidth > 1 ? mipWidth >> 1 : 1, mipHeight > 1 ? mipHeight >> 1 : 1,
//                               1} // each mip level is half the size of the previous level
//         };

//         // src image == dst image, because we're blitting between different mip levels of the same image
//         vkCmdBlitImage(*cmdBuffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture->image,
//                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, mipFilter);

//         imgBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//         imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//         imgBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//         imgBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

//         vkCmdPipelineBarrier(*cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
//                              NULL, 0, NULL, 1, &imgBarrier);

//         // avoid zero-sized mip levels
//         if (mipWidth > 1)
//             mipWidth >>= 1;
//         if (mipHeight > 1)
//             mipHeight >>= 1;
//     }

//     imgBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//     imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//     imgBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//     imgBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//     imgBarrier.subresourceRange.baseMipLevel = texture->mipLevels - 1;

//     vkCmdPipelineBarrier(*cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
//     NULL,
//                          0, NULL, 1, &imgBarrier);
// }

// internal helper
// static void createTextureImage(qvktexture_t *dstTex, const unsigned char *data, uint32_t width, uint32_t height)
// {
//     int unifiedTransferAndGfx = vk_device.transferQueue == vk_device.gfxQueue ? 1 : 0;
//     // assuming 32bit images
//     uint32_t imageSize = width * height * 4;

//     VkBuffer staging_buffer;
//     VkCommandBuffer command_buffer;
//     uint32_t staging_offset;
//     void *imgData = QVk_GetStagingBuffer(imageSize, 4, &command_buffer, &staging_buffer, &staging_offset);
//     memcpy(imgData, data, (size_t)imageSize);

//     VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
//     // set extra image usage flag if we're dealing with mipmapped image - will need it for copying data between mip
//     // levels
//     if (dstTex->mipLevels > 1)
//         imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

//     VK_VERIFY(QVk_CreateImage(width, height, dstTex->format, VK_IMAGE_TILING_OPTIMAL, imageUsage,
//                               VMA_MEMORY_USAGE_GPU_ONLY, dstTex));

//     transitionImageLayout(&command_buffer, &vk_device.transferQueue, dstTex, VK_IMAGE_LAYOUT_UNDEFINED,
//                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//     // copy buffer to image
//     VkBufferImageCopy region = {.bufferOffset = staging_offset,
//                                 .bufferRowLength = 0,
//                                 .bufferImageHeight = 0,
//                                 .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//                                 .imageSubresource.mipLevel = 0,
//                                 .imageSubresource.baseArrayLayer = 0,
//                                 .imageSubresource.layerCount = 1,
//                                 .imageOffset = {0, 0, 0},
//                                 .imageExtent = {width, height, 1}};

//     vkCmdCopyBufferToImage(command_buffer, staging_buffer, dstTex->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
//                            &region);

//     if (dstTex->mipLevels > 1)
//     {
//         // vkCmdBlitImage requires a queue with GRAPHICS_BIT present
//         generateMipmaps(&command_buffer, dstTex, width, height);
//     }
//     else
//     {
//         // for non-unified transfer and graphics, this step begins queue ownership transfer to graphics queue (for
//         // exclusive sharing only)
//         if (unifiedTransferAndGfx || dstTex->sharingMode == VK_SHARING_MODE_EXCLUSIVE)
//             transitionImageLayout(&command_buffer, &vk_device.transferQueue, dstTex,
//                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

//         if (!unifiedTransferAndGfx)
//         {
//             transitionImageLayout(&command_buffer, &vk_device.gfxQueue, dstTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//         }
//     }
// }

// VkResult QVk_CreateImageView(const VkImage *image, VkImageAspectFlags aspectFlags, VkImageView *imageView,
//                              VkFormat format, uint32_t mipLevels)
// {
//     VkImageViewCreateInfo ivCreateInfo = {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
//                                           .pNext = NULL,
//                                           .flags = 0,
//                                           .image = *image,
//                                           .viewType = VK_IMAGE_VIEW_TYPE_2D,
//                                           .format = format,
//                                           .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
//                                           .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
//                                           .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
//                                           .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
//                                           .subresourceRange.aspectMask = aspectFlags,
//                                           .subresourceRange.baseArrayLayer = 0,
//                                           .subresourceRange.baseMipLevel = 0,
//                                           .subresourceRange.layerCount = 1,
//                                           .subresourceRange.levelCount = mipLevels};

//     return vkCreateImageView(vk_device.logical, &ivCreateInfo, NULL, imageView);
// }

// VkResult QVk_CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
//                          VkImageUsageFlags usage, VmaMemoryUsage memUsage, qvktexture_t *texture)
// {
//     VkImageCreateInfo imageInfo = {.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
//                                    .imageType = VK_IMAGE_TYPE_2D,
//                                    .extent.width = width,
//                                    .extent.height = height,
//                                    .extent.depth = 1,
//                                    .mipLevels = texture->mipLevels,
//                                    .arrayLayers = 1,
//                                    .format = format,
//                                    .tiling = tiling,
//                                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//                                    .usage = usage,
//                                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
//                                    .samples = texture->sampleCount,
//                                    .flags = 0};

//     uint32_t queueFamilies[] = {(uint32_t)vk_device.gfxFamilyIndex, (uint32_t)vk_device.transferFamilyIndex};
//     if (vk_device.gfxFamilyIndex != vk_device.transferFamilyIndex)
//     {
//         imageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
//         imageInfo.queueFamilyIndexCount = 2;
//         imageInfo.pQueueFamilyIndices = queueFamilies;
//     }

//     VmaAllocationCreateInfo vmallocInfo = {.flags = texture->vmaFlags, .usage = memUsage};

//     texture->sharingMode = imageInfo.sharingMode;
//     return vmaCreateImage(vk_malloc, &imageInfo, &vmallocInfo, &texture->image, &texture->allocation,
//                           &texture->allocInfo);
// }

// void QVk_CreateDepthBuffer(VkSampleCountFlagBits sampleCount, qvktexture_t *depthBuffer)
// {
//     depthBuffer->format = QVk_FindDepthFormat();
//     depthBuffer->sampleCount = sampleCount;
//     // On 64-bit builds, Intel drivers throw a warning:
//     // "Mapping an image with layout VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL can result in undefined
//     behavior
//     // if this memory is used by the device. Only GENERAL or PREINITIALIZED should be used." Minor annoyance but we
//     // don't want any validation warnings, so we create dedicated allocation for depth buffer. more details:
//     // https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/issues/34 Note that this is a false positive
//     // which in other cases could be ignored:
//     //
//     https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/general_considerations.html#general_considerations_validation_layer_warnings
//     depthBuffer->vmaFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

//     VK_VERIFY(QVk_CreateImage(vk_swapchain.extent.width, vk_swapchain.extent.height, depthBuffer->format,
//                               VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
//                               VMA_MEMORY_USAGE_GPU_ONLY, depthBuffer));
//     VK_VERIFY(QVk_CreateImageView(&depthBuffer->image, getDepthStencilAspect(depthBuffer->format),
//                                   &depthBuffer->imageView, depthBuffer->format, depthBuffer->mipLevels));
// }

// void QVk_CreateColorBuffer(VkSampleCountFlagBits sampleCount, qvktexture_t *colorBuffer, int extraFlags)
// {
//     colorBuffer->format = vk_swapchain.format;
//     colorBuffer->sampleCount = sampleCount;
//     // On 64-bit builds, Intel drivers throw a warning:
//     // "Mapping an image with layout VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL can result in undefined behavior if
//     this
//     // memory is used by the device. Only GENERAL or PREINITIALIZED should be used." Minor annoyance but we don't
//     want
//     // any validation warnings, so we create dedicated allocation for color buffer. more details:
//     // https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/issues/34 Note that this is a false positive
//     // which in other cases could be ignored:
//     //
//     https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/general_considerations.html#general_considerations_validation_layer_warnings
//     colorBuffer->vmaFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

//     VK_VERIFY(QVk_CreateImage(vk_swapchain.extent.width, vk_swapchain.extent.height, colorBuffer->format,
//                               VK_IMAGE_TILING_OPTIMAL, extraFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
//                               VMA_MEMORY_USAGE_GPU_ONLY, colorBuffer));
//     VK_VERIFY(QVk_CreateImageView(&colorBuffer->image, VK_IMAGE_ASPECT_COLOR_BIT, &colorBuffer->imageView,
//                                   colorBuffer->format, colorBuffer->mipLevels));
// }

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

// void QVk_UpdateTextureData(qvktexture_t *texture, const unsigned char *data, uint32_t offset_x, uint32_t offset_y,
//                            uint32_t width, uint32_t height)
// {
//     int unifiedTransferAndGfx = vk_device.transferQueue == vk_device.gfxQueue ? 1 : 0;
//     // assuming 32bit images
//     uint32_t imageSize = width * height * 4;

//     VkBuffer staging_buffer;
//     VkCommandBuffer command_buffer;
//     uint32_t staging_offset;
//     void *imgData = QVk_GetStagingBuffer(imageSize, 4, &command_buffer, &staging_buffer, &staging_offset);
//     memcpy(imgData, data, (size_t)imageSize);

//     transitionImageLayout(&command_buffer, &vk_device.transferQueue, texture,
//     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
//                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//     // copy buffer to image
//     VkBufferImageCopy region = {.bufferOffset = staging_offset,
//                                 .bufferRowLength = 0,
//                                 .bufferImageHeight = 0,
//                                 .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//                                 .imageSubresource.mipLevel = 0,
//                                 .imageSubresource.baseArrayLayer = 0,
//                                 .imageSubresource.layerCount = 1,
//                                 .imageOffset = {offset_x, offset_y, 0},
//                                 .imageExtent = {width, height, 1}};

//     vkCmdCopyBufferToImage(command_buffer, staging_buffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
//                            &region);

//     if (texture->mipLevels > 1)
//     {
//         // vkCmdBlitImage requires a queue with GRAPHICS_BIT present
//         generateMipmaps(&command_buffer, texture, width, height);
//     }
//     else
//     {
//         // for non-unified transfer and graphics, this step begins queue ownership transfer to graphics queue (for
//         // exclusive sharing only)
//         if (unifiedTransferAndGfx || texture->sharingMode == VK_SHARING_MODE_EXCLUSIVE)
//             transitionImageLayout(&command_buffer, &vk_device.transferQueue, texture,
//                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

//         if (!unifiedTransferAndGfx)
//         {
//             transitionImageLayout(&command_buffer, &vk_device.gfxQueue, texture,
//             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//         }
//     }
// }

// void QVk_ReleaseTexture(qvktexture_t *texture)
// {
//     QVk_SubmitStagingBuffers();
//     vkDeviceWaitIdle(vk_device.logical);

//     if (texture->image != VK_NULL_HANDLE)
//         vmaDestroyImage(vk_malloc, texture->image, texture->allocation);
//     if (texture->imageView != VK_NULL_HANDLE)
//         vkDestroyImageView(vk_device.logical, texture->imageView, NULL);
//     if (texture->descriptorSet != VK_NULL_HANDLE)
//     {
//         vkFreeDescriptorSets(vk_device.logical, vk_descriptorPool, 1, &texture->descriptorSet);
//         vk_config.allocated_sampler_descriptor_set_count--;
//     }

//     texture->image = VK_NULL_HANDLE;
//     texture->imageView = VK_NULL_HANDLE;
//     texture->descriptorSet = VK_NULL_HANDLE;
// }

// void QVk_ReadPixels(uint8_t *dstBuffer, uint32_t width, uint32_t height)
// {
//     qvkbuffer_t buff;
//     VkCommandBuffer cmdBuffer;
//     extern int vk_activeBufferIdx;

//     qvkbufferopts_t buffOpts = {
//         .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
//         .reqMemFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//         .prefMemFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
//         .vmaUsage = VMA_MEMORY_USAGE_CPU_ONLY,
//         // When taking a screenshot on Intel, the Linux driver may throw a warning:
//         // "Mapping an image with layout VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL can result in undefined behavior if
//         // this memory is used by the device. Only GENERAL or PREINITIALIZED should be used." Minor annoyance but we
//         // don't want any validation warnings, so we create dedicated allocation for the image buffer. more details:
//         // https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/issues/34 Note that this is a false
//         // positive which in other cases could be ignored:
//         //
//         https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/general_considerations.html#general_considerations_validation_layer_warnings
//         .vmaFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT};

//     VK_VERIFY(QVk_CreateBuffer(width * height * 4, &buff, buffOpts));
//     cmdBuffer = QVk_CreateCommandBuffer(&vk_commandPool[vk_activeBufferIdx], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
//     VK_VERIFY(QVk_BeginCommand(&cmdBuffer));

//     // transition the current swapchain image to be a source of data transfer to our buffer
//     VkImageMemoryBarrier imgBarrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//                                        .pNext = NULL,
//                                        .srcAccessMask =
//                                            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
//                                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
//                                        .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
//                                        .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
//                                        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
//                                        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//                                        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//                                        .image = vk_swapchain.images[vk_activeBufferIdx],
//                                        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//                                        .subresourceRange.baseMipLevel = 0,
//                                        .subresourceRange.baseArrayLayer = 0,
//                                        .subresourceRange.layerCount = 1,
//                                        .subresourceRange.levelCount = 1};

//     vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
//     0,
//                          NULL, 0, NULL, 1, &imgBarrier);

//     VkBufferImageCopy region = {.bufferOffset = 0,
//                                 .bufferRowLength = width,
//                                 .bufferImageHeight = height,
//                                 .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//                                 .imageSubresource.mipLevel = 0,
//                                 .imageSubresource.baseArrayLayer = 0,
//                                 .imageSubresource.layerCount = 1,
//                                 .imageOffset = {0, 0, 0},
//                                 .imageExtent = {width, height, 1}};

//     // copy the swapchain image
//     vkCmdCopyImageToBuffer(cmdBuffer, vk_swapchain.images[vk_activeBufferIdx], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
//                            buff.buffer, 1, &region);
//     VK_VERIFY(vkDeviceWaitIdle(vk_device.logical));
//     QVk_SubmitCommand(&cmdBuffer, &vk_device.gfxQueue);

//     // store image in destination buffer
//     memcpy(dstBuffer, (uint8_t *)buff.allocInfo.pMappedData, width * height * 4);

//     QVk_FreeBuffer(&buff);
// }

/*
===============
Vk_ImageList_f
===============
*/
void Vk_ImageList_f(void)
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

        texels += image.upload_width * image.upload_height;

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

        LOGF(LogLevel::eINFO, " %3i %3i RGB: %s\n", image.upload_width, image.upload_height, name);
    }
    LOGF(LogLevel::eINFO, "Total texel count (not counting mipmaps): %i\n", texels);
}

/*
=================================================================

PCX LOADING

=================================================================
*/

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
        *palette = (byte *)malloc(768);
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

/*
=============
LoadTGA
=============
*/
void LoadTGA(char *name, byte **pic, int *width, int *height)
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

    targa_rgba = (byte *)malloc(numPixels * 4);
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

//=======================================================

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

/*
================
Vk_LoadPic

This is also used as an entry point for the generated r_notexture
================
*/
image_t *GRA_LoadPic(const std::string& name, byte *pic, int width, int height, imagetype_t type, int bits)
{
   	textures.at(name) = image_t{
        .name = name,
        .type = type,
        .width = width,
        .height = height,
        .registration_sequence = registration_sequence,
    };

	auto *image = &textures.at(name);

    if (type == it_skin && bits == 8)
    {
        R_FloodFillSkin(pic, width, height);
    }

    if (bits == 8)
    {
        auto mappedData = GRA_MapPalleteImage(pic, width, height);
        GRA_CreateTexture(image->texture, image->name, (unsigned char *)mappedData, image->upload_width,
                          image->upload_height);
        tf_free(mappedData);
    }
    else
    {
        GRA_CreateTexture(image->texture, image->name, (unsigned char *)pic, image->upload_width, image->upload_height);
    }

    return image;
}

/*
================
Vk_LoadWal
================
*/
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

/*
===============
GRA_FindImage

Finds or loads the given image
===============
*/
image_t *GRA_FindImage(std::string name, imagetype_t type)
{
    int i, len;
    byte *pic, *palette;
    int width, height;
    image_t* image;

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
        free(pic);
    }
    if (palette)
    {
        free(palette);
    }

    return image;
}

/*
===============
R_RegisterSkin
===============
*/
struct image_s *R_RegisterSkin(char *name)
{
    return GRA_FindImage(name, it_skin);
}

/*
================
Vk_FreeUnusedImages

Any image that was not touched on this registration sequence
will be freed.
================
*/
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
        Sys_Error(ERR_FATAL, "Couldn't load pics/colormap.pcx");

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

/*
===============
Vk_InitImages
===============
*/
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

/*
===============
Vk_ShutdownImages
===============
*/
void GRA_ShutdownImages(void)
{
    for (auto &pair : textures)
    {
        auto &[name, image] = pair;
        removeResource(image.texture);
    }

    // QVk_ReleaseTexture(&vk_rawTexture);

    /*
    for (i = 0; i < MAX_LIGHTMAPS * 2; i++)
        QVk_ReleaseTexture(&vk_state.lightmap_textures[i]);
    */
}
