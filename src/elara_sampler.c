/*
** EPITECH PROJECT 2022
** elara_sampler.c
** File description:
** Sampler implementation
*/

#include "elara_vulkan.h"

u32 SamplerAddressModeToVulkan(sampler_address_mode Mode)
{
    if (Mode == SamplerAddressMode_Repeat)
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    if (Mode == SamplerAddressMode_MirroredRepeat)
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    if (Mode == SamplerAddressMode_ClampToEdge)
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    if (Mode == SamplerAddressMode_ClampToBorder)
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

u32 FilterToVulkan(sampler_filter Filter)
{
    if (Filter == SamplerFilter_Nearest)
        return VK_FILTER_NEAREST;
    if (Filter == SamplerFilter_Linear)
        return VK_FILTER_LINEAR;
    return VK_FILTER_NEAREST;
}

void SamplerInit(gpu_sampler* Sampler, u32 Mips)
{
    u32 AddressMode = SamplerAddressModeToVulkan(Sampler->AddressMode);
    u32 Filter = FilterToVulkan(Sampler->Filter);
    
    VkSamplerCreateInfo CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    CreateInfo.magFilter = Filter;
    CreateInfo.minFilter = Filter;
    CreateInfo.addressModeU = AddressMode;
    CreateInfo.addressModeV = AddressMode;
    CreateInfo.addressModeW = AddressMode;
    CreateInfo.anisotropyEnable = VK_TRUE;
    CreateInfo.maxAnisotropy = 16;
    CreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    CreateInfo.compareEnable = VK_FALSE;
    CreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    CreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    CreateInfo.unnormalizedCoordinates = VK_FALSE;
    CreateInfo.minLod = 0.0f;
    CreateInfo.maxLod = (f32)Mips;
    CreateInfo.mipLodBias = 0.0f;
    
    VkResult Result = vkCreateSampler(VulkanState.Device, &CreateInfo, NULL, &Sampler->Sampler);
    CheckVk(Result, "Failed to create sampler!");
}

void SamplerFree(gpu_sampler* Sampler)
{
    vkDestroySampler(VulkanState.Device, Sampler->Sampler, NULL);
}