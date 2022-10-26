/*
** EPITECH PROJECT 2022
** elara_vulkan.h
** File description:
** Vulkan header for Elara
*/

#ifndef ELARA_VULKAN_H
#define ELARA_VULKAN_H

#include "elara_common.h"
#include "elara_platform.h"
#include "elara_vma.h"

#include <vulkan/vulkan.h>

#define FRAMES_IN_FLIGHT 2

typedef enum command_buffer_type {
    CommandBufferType_Graphics,
    CommandBufferType_Compute,
    CommandBufferType_Upload
} command_buffer_type;

typedef enum gpu_buffer_usage {
    GpuBufferUsage_Vertex,
    GpuBufferUsage_Index,
    GpuBufferUsage_Uniform
} gpu_buffer_usage;

typedef struct command_buffer {
    VkCommandBuffer Handle;
    command_buffer_type CommandBufferType;
} command_buffer;

typedef struct gpu_buffer {
    VkBuffer Buffer;
    VmaAllocation Allocation;
    VkBufferUsageFlagBits Usage;
    VmaMemoryUsage MemoryUsage;
} gpu_buffer;

typedef struct vulkan_state {
    VkInstance Instance;
    char* Layers[64];
    char* Extensions[64];
    u32 LayerCount;
    u32 ExtensionCount;
    
    VkPhysicalDevice GPU;
    u32 GraphicsFamily;
    u32 ComputeFamily;
    
    VkSurfaceKHR Surface;
    
    VkDevice Device;
    char* DeviceExtensions[64];
    u32 DeviceExtensionCount;
    VkQueue GraphicsQueue;
    VkQueue ComputeQueue;
    VkCommandPool GraphicsPool;
    VkCommandPool ComputePool;
    VkCommandPool UploadPool;
    
    VkSwapchainKHR Swapchain;
    VkExtent2D SwapchainExtent;
    VkFormat SwapchainFormat;
    VkImage* SwapchainImages;
    VkImageView SwapchainImageViews[FRAMES_IN_FLIGHT];
    command_buffer SwapchainCommandBuffers[FRAMES_IN_FLIGHT];
    
    VkFence UploadFence;
    VkFence ComputeFence;
    VkFence SwapchainFences[FRAMES_IN_FLIGHT];
    VkSemaphore AvailableSemaphore;
    VkSemaphore RenderedSemaphore;
    i32 ImageIndex;
    
    VmaAllocator Allocator;
} vulkan_state;

extern vulkan_state VulkanState;

void CheckVk(VkResult Result, const char* Message);
void InitVulkan();
void ExitVulkan();
void ResizeVulkan();

// Render control
void RenderBegin();
void RenderEnd();
void RenderPresent();

// Command Buffer
void CommandBufferInit(command_buffer* CommandBuffer, command_buffer_type Type);
void CommandBufferFree(command_buffer* CommandBuffer);
void CommandBufferBegin(command_buffer* CommandBuffer);
void CommandBufferEnd(command_buffer* CommandBuffer);
void CommandBufferSubmit(command_buffer* CommandBuffer);

// GPU buffer
void BufferAllocate(gpu_buffer* Buffer, u64 Size, gpu_buffer_usage Usage);
void BufferFree(gpu_buffer* Buffer);
void BufferUpload(gpu_buffer* Buffer, const void* Data, u64 Size);

// Defined in platform files
void PlatformCreateSurface(VkInstance Instance, VkSurfaceKHR* Surface);

#endif //ELARA_VULKAN_H
