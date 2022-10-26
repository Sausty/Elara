/*
** EPITECH PROJECT, 2022
** elara_gpu_buffer.c
** File description:
** A GPU buffer implementation
*/

#include "elara_vulkan.h"

VkBufferUsageFlagBits TypeToUsage(gpu_buffer_usage Usage)
{
    if (Usage == GpuBufferUsage_Vertex)
        return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (Usage == GpuBufferUsage_Index)
        return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (Usage == GpuBufferUsage_Uniform)
        return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
}

u32 GetMemoryUsage(VkBufferUsageFlagBits Flags)
{
    VkBufferUsageFlagBits Vertex = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    VkBufferUsageFlagBits Index = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    VkBufferUsageFlagBits Uniform = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    
    if (Flags == Vertex)
        return VMA_MEMORY_USAGE_CPU_TO_GPU;
    if (Flags == Index)
        return VMA_MEMORY_USAGE_CPU_TO_GPU;
    if (Flags == Uniform)
        return VMA_MEMORY_USAGE_CPU_ONLY;
    return 0;
}

void BufferAllocate(gpu_buffer* Buffer, u64 Size, gpu_buffer_usage Usage)
{
    Buffer->Usage = TypeToUsage(Usage);
    
    VkBufferCreateInfo BufferCreateInfo = {0};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size = Size;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    BufferCreateInfo.usage = TypeToUsage(Usage);
    
    VmaAllocationCreateInfo AllocationCreateInfo = {0};
    AllocationCreateInfo.usage = GetMemoryUsage(BufferCreateInfo.usage);
    
    VkResult Result = vmaCreateBuffer(VulkanState.Allocator, &BufferCreateInfo, &AllocationCreateInfo, &Buffer->Buffer, &Buffer->Allocation, NULL);
    CheckVk(Result, "Failed to allocate VMA buffer!");
}

void BufferFree(gpu_buffer* Buffer)
{
    vmaDestroyBuffer(VulkanState.Allocator, Buffer->Buffer, Buffer->Allocation);
}

void BufferUpload(gpu_buffer* Buffer, const void* Data, u64 Size)
{
    void* Pointer = NULL;
    CheckVk(vmaMapMemory(VulkanState.Allocator, Buffer->Allocation, &Pointer), "Failed to map buffer memory!");
    memcpy(Pointer, Data, Size);
    vmaUnmapMemory(VulkanState.Allocator, Buffer->Allocation);
}