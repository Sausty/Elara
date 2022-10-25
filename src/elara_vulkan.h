/*
** EPITECH PROJECT 2022
** elara_vulkan.h
** File description:
** Vulkan header for Elara
*/

#ifndef ELARA_VULKAN_H
#define ELARA_VULKAN_H

#include "elara_common.h"

#include <vulkan/vulkan.h>

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
} vulkan_state;

extern vulkan_state VulkanState;

void CheckVk(VkResult Result, const char* Message);
void InitVulkan();
void ExitVulkan();

// Defined in platform files
void PlatformCreateSurface(VkInstance Instance, VkSurfaceKHR* Surface);

#endif //ELARA_VULKAN_H
