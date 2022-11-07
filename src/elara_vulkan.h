/*
** EPITECH PROJECT 2022
** elara_vulkan.h
** File description:
** Vulkan header for Elara
*/

// TODO(milo): Hello triangle
// TODO(milo): Hello cube
// TODO(milo): Mesh loading
// TODO(milo): Actual mesh shaders
// TODO(milo): GPU driven culling
// TODO(milo): LOD loading
// TODO(milo): GPU driven LOD selection
// TODO(milo): Multi draw indirect
// TODO(milo): PBR
// TODO(milo): Cleanup and optimizations
// TODO(milo): Comment code

#ifndef ELARA_VULKAN_H
#define ELARA_VULKAN_H

#include "elara_common.h"
#include "elara_platform.h"
#include "elara_vma.h"

#include <vulkan/vulkan.h>

#define FRAMES_IN_FLIGHT 2

typedef enum sampler_address_mode {
    SamplerAddressMode_Repeat,
    SamplerAddressMode_MirroredRepeat,
    SamplerAddressMode_ClampToEdge,
    SamplerAddressMode_ClampToBorder
} sampler_address_mode;

typedef enum sampler_filter {
    SamplerFilter_Nearest,
    SamplerFilter_Linear
} sampler_filter;

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

typedef enum gpu_descriptor_type {
    GpuDescriptorType_Image,
    GpuDescriptorType_SampledImage,
    GpuDescriptorType_Sampler,
    GpuDescriptorType_Buffer,
    GpuDescriptorType_StorageImage
} gpu_descriptor_type;

typedef enum gpu_pipeline_type {
    GpuPipelineType_Graphics,
    GpuPipelineType_Compute
} gpu_pipeline_type;

typedef struct command_buffer {
    VkCommandBuffer Handle;
    command_buffer_type CommandBufferType;
} command_buffer;

typedef struct gpu_buffer {
    VkBuffer Buffer;
    VmaAllocation Allocation;
    VkBufferUsageFlagBits Usage;
    VmaMemoryUsage MemoryUsage;
    u64 DeviceAddress;
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

typedef struct gpu_sampler {
    sampler_filter Filter;
    sampler_address_mode AddressMode;
    VkSampler Sampler;
} gpu_sampler;

typedef struct gpu_descriptor_set_layout {
    gpu_descriptor_type Descriptors[32];
    u32 DescriptorCount;
    
    VkDescriptorSetLayout Layout;
} gpu_descriptor_set_layout;

typedef struct gpu_descriptor_set {
    VkDescriptorSet Set;
} gpu_descriptor_set;

typedef struct gpu_descriptor_heap {
    gpu_descriptor_type Type;
    u32 Length;
    u32 Capacity;
    b8* HeapHandle;
    VkDescriptorSetLayout Layout;
    VkDescriptorSet Descriptor;
} gpu_descriptor_heap;

typedef struct shader_module {
    VkShaderModule Shader;
    u32* ByteCode;
    u32 ByteCodeSize;
} shader_module;

typedef struct gpu_pipeline_descriptor {
    b8 UseMS;
    b8 ReflectInputLayout;
    b8 WindingOrderCCW;
    
    struct {
        shader_module* Vertex;
        shader_module* Fragment;
        shader_module* Compute;
        shader_module* Mesh;
        shader_module* Task;
        
        u32 PushConstantSize;
        
        gpu_descriptor_set_layout Layouts[16];
        u32 SetLayoutCount;
    } ShaderInfo;
    
    VkPrimitiveTopology PrimitiveTopology;
    VkPolygonMode PolygonMode;
    VkCullModeFlagBits CullMode;
    VkFrontFace FrontFace;
    VkCompareOp DepthCompare;
    
    i32 ColorAttachmentCount;
    VkFormat ColorFormats[32];
    VkFormat DepthFormat;
} gpu_pipeline_descriptor;

typedef struct gpu_pipeline {
    gpu_pipeline_type PipelineType;
    VkPipelineBindPoint BindPoint;
    VkPipeline Pipeline;
    VkPipelineLayout PipelineLayout;
} gpu_pipeline;

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
    VkPhysicalDeviceFeatures2 Features;
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
    VkDescriptorPool DescriptorPool;
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

// GPU sampler
void SamplerInit(gpu_sampler* Sampler, u32 Mips);
void SamplerFree(gpu_sampler* Sampler);

// Descriptor set layout
void DescriptorSetLayoutInit(gpu_descriptor_set_layout* Layout);
void DescriptorSetLayoutFree(gpu_descriptor_set_layout* Layout);

// Descriptor set
void DescriptorSetInit(gpu_descriptor_set* Set, gpu_descriptor_set_layout* Layout);
void DescriptorSetFree(gpu_descriptor_set* Set);
void DescriptorSetWriteSampler(gpu_descriptor_set* Set, gpu_sampler* Sampler, i32 Binding);
void DescriptorSetWriteBuffer(gpu_descriptor_set* Set, gpu_buffer* Buffer, u64 Size, i32 Binding);
void DescriptorSetWriteStorageBuffer(gpu_descriptor_set* Set, gpu_buffer* Buffer, u64 Size, i32 Binding);
void DescriptorSetWriteImage(gpu_descriptor_set* Set, gpu_image* Image, i32 Binding);
void DescriptorSetWriteImageSampler(gpu_descriptor_set* Set, gpu_image* Image, gpu_sampler* Sampler, i32 Binding);
void DescriptorSetWriteStorageImage(gpu_descriptor_set* Set, gpu_image* Image, gpu_sampler* Sampler, i32 Binding);

// Descriptor heap
void DescriptorHeapInit(gpu_descriptor_heap* Heap, gpu_descriptor_type Type, u32 Capacity);
void DescriptorHeapFree(gpu_descriptor_heap* Heap);
u32 DescriptorHeapPushImage(gpu_descriptor_heap* Heap, gpu_image* Image);
u32 DescriptorHeapPushSampler(gpu_descriptor_heap* Heap, gpu_sampler* Sampler);
u32 DescriptorHeapPushBuffer(gpu_descriptor_heap* Heap, gpu_buffer* Buffer, u64 Size);
void DescriptorHeapFreeDescriptor(gpu_descriptor_heap* Heap, u32 Index);

// Shader module
void ShaderLoad(shader_module* Shader, const char* Path);
void ShaderFree(shader_module* Shader);

// Pipeline
void PipelineInitGraphics(gpu_pipeline* Pipeline, gpu_pipeline_descriptor* Descriptor);
void PipelineInitCompute(gpu_pipeline* Pipeline, gpu_pipeline_descriptor* Descriptor);
void PipelineFree(gpu_pipeline* Pipeline);

// Defined in platform files
void PlatformCreateSurface(VkInstance Instance, VkSurfaceKHR* Surface);

#endif 