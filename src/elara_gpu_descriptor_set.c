/*
** EPITECH PROJECT 2022
** elara_gpu_descriptor_set.c
** File description:
** Descriptor set layout and descriptor set implementation
*/

#include "elara_vulkan.h"

VkDescriptorType DescriptorTypeToVulkan(gpu_descriptor_type Type)
{
    if (Type == GpuDescriptorType_Image)
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    if (Type == GpuDescriptorType_SampledImage)
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    if (Type == GpuDescriptorType_Sampler)
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    if (Type == GpuDescriptorType_Buffer)
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    if (Type == GpuDescriptorType_StorageImage)
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
}

void DescriptorSetLayoutInit(gpu_descriptor_set_layout* Layout)
{
    VkDescriptorSetLayoutBinding Bindings[32];
    
    for (i32 Binding = 0; Binding < Layout->DescriptorCount; Binding++)
    {
        Bindings[Binding].binding = Binding;
        Bindings[Binding].descriptorCount = 1;
        Bindings[Binding].descriptorType = DescriptorTypeToVulkan(Layout->Descriptors[Binding]);
        Bindings[Binding].stageFlags = VK_SHADER_STAGE_ALL;
    }
    
    VkDescriptorSetLayoutCreateInfo CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    CreateInfo.pBindings = Bindings;
    CreateInfo.bindingCount = Layout->DescriptorCount;
    
    VkResult Result = vkCreateDescriptorSetLayout(VulkanState.Device, &CreateInfo, NULL, &Layout->Layout);
    CheckVk(Result, "Failed to create descriptor set layout!");
}

void DescriptorSetLayoutFree(gpu_descriptor_set_layout* Layout)
{
    vkDestroyDescriptorSetLayout(VulkanState.Device, Layout->Layout, NULL);
}

void DescriptorSetInit(gpu_descriptor_set* Set, gpu_descriptor_set_layout* Layout)
{
    VkDescriptorSetAllocateInfo AllocInfo = {0};
    AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocInfo.descriptorPool = VulkanState.DescriptorPool;
    AllocInfo.pSetLayouts = &Layout->Layout;
    AllocInfo.descriptorSetCount = 1;
    
    VkResult Result = vkAllocateDescriptorSets(VulkanState.Device, &AllocInfo, &Set->Set);
    CheckVk(Result, "Failed to allocate descriptor set!");
}

void DescriptorSetFree(gpu_descriptor_set* Set)
{
    vkFreeDescriptorSets(VulkanState.Device, VulkanState.DescriptorPool, 1, &Set->Set);
}

void DescriptorSetWriteBuffer(gpu_descriptor_set* Set, gpu_buffer* Buffer, u64 Size, i32 Binding)
{
    VkDescriptorBufferInfo BufferInfo = {0};
    BufferInfo.buffer = Buffer->Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = Size;
    
    VkWriteDescriptorSet WriteInfo = {0};
    WriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteInfo.dstSet = Set->Set;
    WriteInfo.dstBinding = Binding;
    WriteInfo.descriptorCount = 1;
    WriteInfo.dstArrayElement = 0;
    WriteInfo.pBufferInfo = &BufferInfo;
    WriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    
    vkUpdateDescriptorSets(VulkanState.Device, 1, &WriteInfo, 0, NULL);
}

void DescriptorSetWriteStorageBuffer(gpu_descriptor_set* Set, gpu_buffer* Buffer, u64 Size, i32 Binding)
{
    VkDescriptorBufferInfo BufferInfo = {0};
    BufferInfo.buffer = Buffer->Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = Size;
    
    VkWriteDescriptorSet WriteInfo = {0};
    WriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteInfo.dstSet = Set->Set;
    WriteInfo.dstBinding = Binding;
    WriteInfo.descriptorCount = 1;
    WriteInfo.dstArrayElement = 0;
    WriteInfo.pBufferInfo = &BufferInfo;
    WriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    
    vkUpdateDescriptorSets(VulkanState.Device, 1, &WriteInfo, 0, NULL);   
}

void DescriptorSetWriteImage(gpu_descriptor_set* Set, gpu_image* Image, i32 Binding)
{
    VkDescriptorImageInfo ImageInfo = {0};
    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    ImageInfo.imageView = Image->ImageView;
    ImageInfo.sampler = VK_NULL_HANDLE;
    
    VkWriteDescriptorSet WriteInfo = {0};
    WriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteInfo.dstSet = Set->Set;
    WriteInfo.dstBinding = Binding;
    WriteInfo.descriptorCount = 1;
    WriteInfo.dstArrayElement = 0;
    WriteInfo.pImageInfo = &ImageInfo;
    WriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    
    vkUpdateDescriptorSets(VulkanState.Device, 1, &WriteInfo, 0, NULL);
}

void DescriptorSetWriteImageSampler(gpu_descriptor_set* Set, gpu_image* Image, gpu_sampler* Sampler, i32 Binding)
{
    VkDescriptorImageInfo ImageInfo = {0};
    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    ImageInfo.imageView = Image->ImageView;
    ImageInfo.sampler = Sampler->Sampler;
    
    VkWriteDescriptorSet WriteInfo = {0};
    WriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteInfo.dstSet = Set->Set;
    WriteInfo.dstBinding = Binding;
    WriteInfo.descriptorCount = 1;
    WriteInfo.dstArrayElement = 0;
    WriteInfo.pImageInfo = &ImageInfo;
    WriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    
    vkUpdateDescriptorSets(VulkanState.Device, 1, &WriteInfo, 0, NULL);
}

void DescriptorSetWriteStorageImage(gpu_descriptor_set* Set, gpu_image* Image, gpu_sampler* Sampler, i32 Binding)
{
    VkDescriptorImageInfo ImageInfo = {0};
    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    ImageInfo.imageView = Image->ImageView;
    ImageInfo.sampler = Sampler->Sampler;
    
    VkWriteDescriptorSet WriteInfo = {0};
    WriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteInfo.dstSet = Set->Set;
    WriteInfo.dstBinding = Binding;
    WriteInfo.descriptorCount = 1;
    WriteInfo.dstArrayElement = 0;
    WriteInfo.pImageInfo = &ImageInfo;
    WriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    
    vkUpdateDescriptorSets(VulkanState.Device, 1, &WriteInfo, 0, NULL);
}

void DescriptorSetWriteSampler(gpu_descriptor_set* Set, gpu_sampler* Sampler, i32 Binding)
{
    VkDescriptorImageInfo ImageInfo = {0};
    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageInfo.imageView = VK_NULL_HANDLE;
    ImageInfo.sampler = Sampler->Sampler;
    
    VkWriteDescriptorSet WriteInfo = {0};
    WriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteInfo.dstSet = Set->Set;
    WriteInfo.dstBinding = Binding;
    WriteInfo.descriptorCount = 1;
    WriteInfo.dstArrayElement = 0;
    WriteInfo.pImageInfo = &ImageInfo;
    WriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    
    vkUpdateDescriptorSets(VulkanState.Device, 1, &WriteInfo, 0, NULL);
}