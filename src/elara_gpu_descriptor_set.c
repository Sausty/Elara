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

u32 GetAvailableDescriptor(gpu_descriptor_heap* Heap)
{
    for (i32 Descriptor = 0; Descriptor < Heap->Capacity; Descriptor++)
    {
        if (Heap->HeapHandle[Descriptor] == false)
        {
            Heap->HeapHandle[Descriptor] = true;
            Heap->Length++;
            return Descriptor;
        }
    }
    
    return 0;
}

void DescriptorHeapInit(gpu_descriptor_heap* Heap, gpu_descriptor_type Type, u32 Capacity)
{
    memset(Heap, 0, sizeof(gpu_descriptor_heap));
    memset(Heap->HeapHandle, 0, sizeof(b8) * Capacity);
    
    VkDescriptorBindingFlags Flag = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    
    VkDescriptorSetLayoutBinding Binding = {0};
    Binding.binding = 0;
    Binding.descriptorCount = 2048;
    Binding.descriptorType = DescriptorTypeToVulkan(Type);
    Binding.stageFlags = VK_SHADER_STAGE_ALL;
    
    VkDescriptorSetLayoutBindingFlagsCreateInfo BindingFlags = {0};
    BindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    BindingFlags.bindingCount = 1;
    BindingFlags.pBindingFlags = &Flag;
    
    VkDescriptorSetLayoutCreateInfo SetLayoutInfo = {0};
    SetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    SetLayoutInfo.bindingCount = 1;
    SetLayoutInfo.pBindings = &Binding;
    SetLayoutInfo.pNext = &BindingFlags;
    
    VkResult Result = vkCreateDescriptorSetLayout(VulkanState.Device, &SetLayoutInfo, NULL, &Heap->Layout);
    CheckVk(Result, "Failed to create descriptor heap layout!");
    
    Heap->HeapHandle = malloc(sizeof(b8) * Capacity);
    Heap->Type = Type;
    Heap->Length = 0;
    Heap->Capacity = Capacity;
    
    VkDescriptorSetAllocateInfo AllocateInfo = {0};
    AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocateInfo.descriptorSetCount = 1;
    AllocateInfo.descriptorPool = VulkanState.DescriptorPool;
    AllocateInfo.pSetLayouts = &Heap->Layout;
    
    Result = vkAllocateDescriptorSets(VulkanState.Device, &AllocateInfo, &Heap->Descriptor);
    CheckVk(Result, "Failed to allocate descriptor set for descriptor heap!");
}

void DescriptorHeapFree(gpu_descriptor_heap* Heap)
{
    vkFreeDescriptorSets(VulkanState.Device, VulkanState.DescriptorPool, 1, &Heap->Descriptor);
    vkDestroyDescriptorSetLayout(VulkanState.Device, Heap->Layout, NULL);
    free(Heap->HeapHandle);
}

u32 DescriptorHeapPushImage(gpu_descriptor_heap* Heap, gpu_image* Image)
{
    u32 Index = GetAvailableDescriptor(Heap);
    
    VkDescriptorImageInfo ImageInfo = {0};
    ImageInfo.imageLayout = Image->Layout;
    ImageInfo.imageView = Image->ImageView;
    ImageInfo.sampler = VK_NULL_HANDLE;
    
    VkWriteDescriptorSet Write = {0};
    Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    Write.descriptorCount = 1;
    Write.descriptorType = DescriptorTypeToVulkan(Heap->Type);
    Write.dstArrayElement = Index;
    Write.dstBinding = 0;
    Write.dstSet = Heap->Descriptor;
    Write.pImageInfo = &ImageInfo;
    
    vkUpdateDescriptorSets(VulkanState.Device, 1, &Write, 0, NULL);
    
    return Index;
}

u32 DescriptorHeapPushSampler(gpu_descriptor_heap* Heap, gpu_sampler* Sampler)
{
    u32 Index = GetAvailableDescriptor(Heap);
    
    VkDescriptorImageInfo ImageInfo = {0};
    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageInfo.imageView = VK_NULL_HANDLE;
    ImageInfo.sampler = Sampler->Sampler;
    
    VkWriteDescriptorSet Write = {0};
    Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    Write.descriptorCount = 1;
    Write.descriptorType = DescriptorTypeToVulkan(Heap->Type);
    Write.dstArrayElement = Index;
    Write.dstBinding = 0;
    Write.dstSet = Heap->Descriptor;
    Write.pImageInfo = &ImageInfo;
    
    vkUpdateDescriptorSets(VulkanState.Device, 1, &Write, 0, NULL);
    
    return Index;
}

u32 DescriptorHeapPushBuffer(gpu_descriptor_heap* Heap, gpu_buffer* Buffer, u64 Size)
{
    u32 Index = GetAvailableDescriptor(Heap);
    
    VkDescriptorBufferInfo BufferInfo = {0};
    BufferInfo.buffer = Buffer->Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = Size;
    
    VkWriteDescriptorSet Write = {0};
    Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    Write.descriptorCount = 1;
    Write.descriptorType = DescriptorTypeToVulkan(Heap->Type);
    Write.dstArrayElement = Index;
    Write.dstBinding = 0;
    Write.dstSet = Heap->Descriptor;
    Write.pBufferInfo = &BufferInfo;
    
    vkUpdateDescriptorSets(VulkanState.Device, 1, &Write, 0, NULL);
    
    return Index;
}

void DescriptorHeapFreeDescriptor(gpu_descriptor_heap* Heap, u32 Index)
{
    Heap->HeapHandle[Index] = false;
    Heap->Length--;
}