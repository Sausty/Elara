/*
** EPITECH PROJECT 2022
** elara_vulkan.c
** File description:
** Vulkan RHI for Elara
*/

#include "elara_vulkan.h"

vulkan_state VulkanState;

b8 ValidateLayers(u32 CheckCount, char** CheckNames, u32 LayerCount, VkLayerProperties *Properties)
{
    for (u32 CheckIterator = 0; CheckIterator < CheckCount; CheckIterator++) 
    {
        b8 Found = false;
        for (u32 Layer = 0; Layer < LayerCount; Layer++) 
        {
            if (!strcmp(CheckNames[CheckIterator], Properties[Layer].layerName))
            {
                Found = true;
                break;
            }
        }
    }
    return true;
}

void InitInstance()
{
    u32 InstanceExtensionCount = 0;
    u32 InstanceLayerCount = 0;
    u32 ValidationLayerCount = 0;
    char** InstanceValidationLayers = NULL;
    
    char* RequiredLayers[] = 
    {
        "VK_LAYER_KHRONOS_validation"
    };
    
    b8 ValidationFound;
    VkResult Result = vkEnumerateInstanceLayerProperties(&InstanceLayerCount, NULL);
    CheckVk(Result, "Failed to enumerate instance layer properties!");
    InstanceValidationLayers = RequiredLayers;
    
    if (InstanceLayerCount > 0) 
    {
        VkLayerProperties* InstanceLayers = malloc(sizeof(VkLayerProperties) * InstanceLayerCount);
        Result = vkEnumerateInstanceLayerProperties(&InstanceLayerCount, InstanceLayers);
        CheckVk(Result, "Failed to fetch the instance layer properties!");
        
        ValidationFound = ValidateLayers(1, InstanceValidationLayers, InstanceLayerCount, InstanceLayers);
        if (ValidationFound)
        {
            VulkanState.LayerCount = 1;
            VulkanState.Layers[0] = "VK_LAYER_KHRONOS_validation";
            ValidationLayerCount = 1;
        }
        free(InstanceLayers);
    }
    
    Result = vkEnumerateInstanceExtensionProperties(NULL, &InstanceExtensionCount, NULL);
    CheckVk(Result, "Failed to enumerate instance extension properties!");
    
    if (InstanceExtensionCount > 0)
    {
        VkExtensionProperties* InstanceExtensions = malloc(sizeof(VkExtensionProperties) * InstanceExtensionCount);
        Result = vkEnumerateInstanceExtensionProperties(NULL, &InstanceExtensionCount, InstanceExtensions);
        CheckVk(Result, "Failed to fetch instance extension properties");
        
        for (u32 Extension = 0; Extension < InstanceExtensionCount; Extension++) 
        {
            if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, InstanceExtensions[Extension].extensionName)) {
                VulkanState.Extensions[VulkanState.ExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
            }
            
            if (!strcmp(SURFACE_EXTENSION_NAME, InstanceExtensions[Extension].extensionName)) {
                VulkanState.Extensions[VulkanState.ExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
            }
        }
        free(InstanceExtensions);
    }
    
    VkApplicationInfo AppInfo = {0};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = "Elara Application";
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "Elara";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_3;
    
    VkInstanceCreateInfo CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    CreateInfo.pApplicationInfo = &AppInfo;
    CreateInfo.enabledExtensionCount = VulkanState.ExtensionCount;
    CreateInfo.ppEnabledExtensionNames = (const char *const *)VulkanState.Extensions;
    CreateInfo.enabledLayerCount = VulkanState.LayerCount;
    CreateInfo.ppEnabledLayerNames = (const char *const *)VulkanState.Layers;
    
    Result = vkCreateInstance(&CreateInfo, NULL, &VulkanState.Instance);
    CheckVk(Result, "Failed to create vulkan instance!");
}

void InitPhysicalDevice()
{
    u32 DeviceCount = 0;
    vkEnumeratePhysicalDevices(VulkanState.Instance, &DeviceCount, NULL);
    assert(DeviceCount > 0);
    
    VkPhysicalDevice* Devices = malloc(sizeof(VkPhysicalDevice) * DeviceCount);
    vkEnumeratePhysicalDevices(VulkanState.Instance, &DeviceCount, Devices);
    VulkanState.GPU = Devices[0];
    free(Devices);
    
    u32 QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(VulkanState.GPU, &QueueFamilyCount, NULL);
    
    VkQueueFamilyProperties* QueueFamilies = malloc(sizeof(VkQueueFamilyProperties) * QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(VulkanState.GPU, &QueueFamilyCount, QueueFamilies);
    
    for (u32 Family = 0; Family < QueueFamilyCount; Family++)
    {
        if (QueueFamilies[Family].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            VulkanState.GraphicsFamily = Family;
        }
        
        if (QueueFamilies[Family].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            VulkanState.ComputeFamily = Family;
        }
    }
    free(QueueFamilies);
}

void InitDevice()
{
    f32 QueuePriority = 1.0f;
    
    VkDeviceQueueCreateInfo GraphicsQueueInfo = {0};
    GraphicsQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    GraphicsQueueInfo.queueFamilyIndex = VulkanState.GraphicsFamily;
    GraphicsQueueInfo.queueCount = 1;
    GraphicsQueueInfo.pQueuePriorities = &QueuePriority;
    
    VkDeviceQueueCreateInfo ComputeQueueInfo = {0};
    ComputeQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    ComputeQueueInfo.queueFamilyIndex = VulkanState.ComputeFamily;
    ComputeQueueInfo.queueCount = 1;
    ComputeQueueInfo.pQueuePriorities = &QueuePriority;
    
    VkPhysicalDeviceFeatures Features = {0};
    Features.samplerAnisotropy = 1;
    Features.fillModeNonSolid = 1;
    Features.pipelineStatisticsQuery = 1;
    
    u32 ExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(VulkanState.GPU, NULL, &ExtensionCount, NULL);
    VkExtensionProperties* ExtensionProperties = malloc(sizeof(VkExtensionProperties) * ExtensionCount);
    vkEnumerateDeviceExtensionProperties(VulkanState.GPU, NULL, &ExtensionCount, ExtensionProperties);
    
    for (int Extension = 0; Extension < ExtensionCount; Extension++)
    {
        VkExtensionProperties Property = ExtensionProperties[Extension];
       
        if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, Property.extensionName)) 
        {
            VulkanState.DeviceExtensions[VulkanState.DeviceExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        }
    }
    free(ExtensionProperties);
    
    VkDeviceQueueCreateInfo Queues[2] = { GraphicsQueueInfo, ComputeQueueInfo };
    
    VkDeviceCreateInfo CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    CreateInfo.queueCreateInfoCount = 2;
    CreateInfo.pQueueCreateInfos = Queues;
    CreateInfo.enabledExtensionCount = VulkanState.DeviceExtensionCount;
    CreateInfo.ppEnabledExtensionNames = (const char* const*)VulkanState.DeviceExtensions;
    
    VkResult Result = vkCreateDevice(VulkanState.GPU, &CreateInfo, NULL, &VulkanState.Device);
    CheckVk(Result, "Failed to create vulkan device!");
    
    vkGetDeviceQueue(VulkanState.Device, VulkanState.GraphicsFamily, 0, &VulkanState.GraphicsQueue);
    vkGetDeviceQueue(VulkanState.Device, VulkanState.ComputeFamily, 0, &VulkanState.ComputeQueue);
}

void InitVulkan()
{
    InitInstance();
    InitPhysicalDevice();
    PlatformCreateSurface(VulkanState.Instance, &VulkanState.Surface);
    InitDevice();
}

void ExitVulkan()
{
    vkDestroyDevice(VulkanState.Device, NULL);
    vkDestroySurfaceKHR(VulkanState.Instance, VulkanState.Surface, NULL);
    vkDestroyInstance(VulkanState.Instance, NULL);
}

void CheckVk(VkResult Result, const char* Message)
{
    if (Result != VK_SUCCESS)
        printf("%s", Message);
}