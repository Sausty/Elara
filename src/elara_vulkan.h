/*
** EPITECH PROJECT 2022
** elara_vulkan.h
** File description:
** Vulkan header for Elara
*/

// TODO(milo): Render graph
// TODO(milo): Descriptor sets
// TODO(milo): Pipelines
// TODO(milo): Mesh loading
// TODO(milo): Actual mesh shaders
// TODO(milo): GPU driven culling
// TODO(milo): LOD loading
// TODO(milo): GPU driven LOD selection
// TODO(milo): Multi draw indirect
// TODO(milo): PBR

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

typedef enum gpu_image_usage {
    GpuImageUsage_RTV,
    GpuImageUsage_GBuffer,
    GpuImageUsage_Depth,
    GpuImageUsage_Storage
} gpu_image_usage;

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

typedef struct gpu_image {
    VkImage Image;
    VkImageView ImageView;
    VkFormat Format;
    VkExtent2D Extent;
    VkImageLayout Layout;
    
    VmaAllocation Allocation;
    
    u32 Width, Height;
    gpu_image_usage Usage;
    u32 MipLevels;
} gpu_image;

typedef struct dynamic_rendering_info {
    gpu_image* Images[32];
    u32 ImageCount;
    
    u32 Width;
    u32 Height;
    b8 HasDepth;
    b8 ReadDepth;
    b8 ReadColor;
    
    f32 R, G, B, A;
} dynamic_rendering_info;

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
    gpu_image SwapchainImageHandles[FRAMES_IN_FLIGHT];
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
void CommandBufferSetViewport(command_buffer* CommandBuffer, i32 Width, i32 Height);
void CommandBufferSetVertexBuffer(command_buffer* CommandBuffer, gpu_buffer* Buffer);
void CommandBufferSetIndexBuffer(command_buffer* CommandBuffer, gpu_buffer* Buffer);
void CommandBufferTransitionImageLayout(command_buffer* CommandBuffer, gpu_image* Image, u32 SrcA, u32 DstA, u32 SrcL, u32 DstL, u32 SrcS, u32 DstS, u32 Layer);
void CommandBufferBeginDynamicRendering(command_buffer* CommandBuffer, dynamic_rendering_info* Info);
void CommandBufferEndDynamicRendering(command_buffer* CommandBuffer);

// GPU buffer
void BufferAllocate(gpu_buffer* Buffer, u64 Size, gpu_buffer_usage Usage);
void BufferFree(gpu_buffer* Buffer);
void BufferUpload(gpu_buffer* Buffer, const void* Data, u64 Size);

// GPU image
void ImageAllocate(gpu_image* Image, u32 Width, u32 Height, VkFormat Format, gpu_image_usage Usage, u32 TargetLayout);
void ImageFree(gpu_image* Image);
u32 GetImageAspect(u32 Format);

// Defined in platform files
void PlatformCreateSurface(VkInstance Instance, VkSurfaceKHR* Surface);

#endif //ELARA_VULKAN_H
