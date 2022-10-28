/*
** EPITECH PROJECT 2022
** elara_gpu_image.c
** File description:
** GPU image implementation
*/

#include "elara_vulkan.h"

VkImageUsageFlagBits UsageToImage(gpu_image_usage Usage)
{
    if (Usage == GpuImageUsage_RTV)
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (Usage == GpuImageUsage_GBuffer)
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    if (Usage == GpuImageUsage_Depth)
        return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
    if (Usage == GpuImageUsage_Storage)
        return VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    return(0);
}

u32 GetImageAspect(u32 Format)
{
    if (Format == VK_FORMAT_D16_UNORM || Format == VK_FORMAT_D16_UNORM_S8_UINT || Format == VK_FORMAT_D24_UNORM_S8_UINT || Format == VK_FORMAT_D32_SFLOAT || Format == VK_FORMAT_D32_SFLOAT_S8_UINT)
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    
    return(VK_IMAGE_ASPECT_COLOR_BIT);
}

void ImageAllocate(gpu_image* Image, u32 Width, u32 Height, VkFormat Format, gpu_image_usage Usage, u32 TargetLayout)
{
    Image->Width = Width;
    Image->Height = Height;
    Image->Format = Format;
    Image->Usage = Usage;
    Image->Extent.width = Width;
    Image->Extent.height = Height;
    Image->MipLevels = 1;
    
    VkImageCreateInfo ImageInfo = {0};
    ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageInfo.extent.width = Width;
    ImageInfo.extent.height = Height;
    ImageInfo.extent.depth = 1;
    ImageInfo.mipLevels = Image->MipLevels;
    ImageInfo.arrayLayers = 1;
    ImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    ImageInfo.usage = UsageToImage(Usage);
    ImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VmaAllocationCreateInfo AllocationInfo = {0};
    AllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    
    VkResult Result = vmaCreateImage(VulkanState.Allocator, &ImageInfo, &AllocationInfo, &Image->Image, &Image->Allocation, NULL);
    CheckVk(Result, "Failed to allocate image!");
    
    VkImageViewCreateInfo ImageViewInfo = {0};
    ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewInfo.image = Image->Image;
    ImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewInfo.format = Image->Format;
    ImageViewInfo.subresourceRange.aspectMask = GetImageAspect(Image->Format);
    ImageViewInfo.subresourceRange.baseMipLevel = 0;
    ImageViewInfo.subresourceRange.levelCount = Image->MipLevels;
    ImageViewInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewInfo.subresourceRange.layerCount = 1;
    ImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    
    Result = vkCreateImageView(VulkanState.Device, &ImageViewInfo, NULL, &Image->ImageView);
    CheckVk(Result, "Failed to create image view!");
    
    // TODO(milo): Change layout using command buffer
}

void ImageFree(gpu_image* Image)
{
    vkDestroyImageView(VulkanState.Device, Image->ImageView, NULL);
    vmaDestroyImage(VulkanState.Allocator, Image->Image, Image->Allocation);
}