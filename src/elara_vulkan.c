/*
** EPITECH PROJECT 2022
** elara_vulkan.c
** File description:
** Contains all the boilerplate vulkan code (device, swapchain, sync etc)
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

void InitSwapchain()
{
    VulkanState.SwapchainExtent.width = PlatformState.Dimensions.Width;
    VulkanState.SwapchainExtent.height = PlatformState.Dimensions.Height;
    u32 QueueFamilyIndices[] = { VulkanState.GraphicsFamily };
    
    VkSurfaceCapabilitiesKHR Capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanState.GPU, VulkanState.Surface, &Capabilities);
    
    u32 FormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanState.GPU, VulkanState.Surface, &FormatCount, NULL);
    VkSurfaceFormatKHR* Formats = malloc(sizeof(VkSurfaceFormatKHR) * FormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanState.GPU, VulkanState.Surface, &FormatCount, Formats);
    
    VkSwapchainCreateInfoKHR CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    CreateInfo.surface = VulkanState.Surface;
    CreateInfo.minImageCount = FRAMES_IN_FLIGHT;
    CreateInfo.imageExtent = VulkanState.SwapchainExtent;
    CreateInfo.imageArrayLayers = 1;
    CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    CreateInfo.queueFamilyIndexCount = 1;
    CreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
    CreateInfo.preTransform = Capabilities.currentTransform;
    CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    CreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // TODO(milo): Setting to enable/disable vsync?
    CreateInfo.clipped = VK_TRUE;
    CreateInfo.oldSwapchain = VK_NULL_HANDLE;
    CreateInfo.imageFormat = Formats[0].format;
    CreateInfo.imageColorSpace = Formats[0].colorSpace;
    
    VulkanState.SwapchainFormat = CreateInfo.imageFormat;
    
    free(Formats);
    
    VkResult Result = vkCreateSwapchainKHR(VulkanState.Device, &CreateInfo, NULL, &VulkanState.Swapchain);
    CheckVk(Result, "Failed to create Vulkan swapchain!");
    
    u32 ImageCount = 0;
    vkGetSwapchainImagesKHR(VulkanState.Device, VulkanState.Swapchain, &ImageCount, NULL);
    VulkanState.SwapchainImages = malloc(sizeof(VkImage) * ImageCount);
    vkGetSwapchainImagesKHR(VulkanState.Device, VulkanState.Swapchain, &ImageCount, VulkanState.SwapchainImages);
    
    for (u32 ImageIterator = 0; ImageIterator < ImageCount; ImageIterator++)
    {
        VkImage Image = VulkanState.SwapchainImages[ImageIterator];
        VkImageView* ImageView = &VulkanState.SwapchainImageViews[ImageIterator];
        
        VkImageViewCreateInfo IVCreateInfo = {0};
        IVCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        IVCreateInfo.image = Image;
        IVCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        IVCreateInfo.format = CreateInfo.imageFormat;
        IVCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        IVCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        IVCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        IVCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        IVCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        IVCreateInfo.subresourceRange.baseMipLevel = 0;
        IVCreateInfo.subresourceRange.levelCount = 1;
        IVCreateInfo.subresourceRange.baseArrayLayer = 0;
        IVCreateInfo.subresourceRange.layerCount = 1;
        
        Result = vkCreateImageView(VulkanState.Device, &IVCreateInfo, NULL, ImageView);
        CheckVk(Result, "Failed to create swapchain image view!");
    }
}

void InitSync()
{
    VkFenceCreateInfo FenceInfo = {0};
    FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    
    VkResult Result = vkCreateFence(VulkanState.Device, &FenceInfo, NULL, &VulkanState.UploadFence);
    CheckVk(Result, "Failed to create upload fence!");
    
    Result = vkCreateFence(VulkanState.Device, &FenceInfo, NULL, &VulkanState.ComputeFence);
    CheckVk(Result, "Failed to create compute fence!");
    
    FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (i32 Frame = 0; Frame < FRAMES_IN_FLIGHT; Frame++)
    {
        Result = vkCreateFence(VulkanState.Device, &FenceInfo, NULL, &VulkanState.SwapchainFences[Frame]);
        CheckVk(Result, "Failed to create frame in flight fence!");
    }
    
    VkSemaphoreCreateInfo SemaphoreInfo = {0};
    SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    Result = vkCreateSemaphore(VulkanState.Device, &SemaphoreInfo, NULL, &VulkanState.AvailableSemaphore);
    CheckVk(Result, "Failed to create image available semaphore!");
    
    Result = vkCreateSemaphore(VulkanState.Device, &SemaphoreInfo, NULL, &VulkanState.RenderedSemaphore);
    CheckVk(Result, "Failed to create image rendered semaphore!");
}

void InitCommand()
{
    VkCommandPoolCreateInfo CommandPoolCreateInfo = {0};
    CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolCreateInfo.queueFamilyIndex = VulkanState.GraphicsFamily;
    CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    VkResult Result = vkCreateCommandPool(VulkanState.Device, &CommandPoolCreateInfo, NULL, &VulkanState.GraphicsPool);
    CheckVk(Result, "Failed to create graphics command pool!");
    
    Result = vkCreateCommandPool(VulkanState.Device, &CommandPoolCreateInfo, NULL, &VulkanState.UploadPool);
    CheckVk(Result, "Failed to create upload command pool!");
    
    CommandPoolCreateInfo.queueFamilyIndex = VulkanState.ComputeFamily;
    Result = vkCreateCommandPool(VulkanState.Device, &CommandPoolCreateInfo, NULL, &VulkanState.ComputePool);
    CheckVk(Result, "Failed to create compute command pool!");
}

void InitVulkan()
{
    InitInstance();
    InitPhysicalDevice();
    PlatformCreateSurface(VulkanState.Instance, &VulkanState.Surface);
    InitDevice();
    InitSwapchain();
    InitSync();
    InitCommand();
}

void ExitVulkan()
{    
    vkDeviceWaitIdle(VulkanState.Device);
    
    vkDestroyCommandPool(VulkanState.Device, VulkanState.ComputePool, NULL);
    vkDestroyCommandPool(VulkanState.Device, VulkanState.UploadPool, NULL);
    vkDestroyCommandPool(VulkanState.Device, VulkanState.GraphicsPool, NULL);
    
    for (u32 Frame = 0; Frame < FRAMES_IN_FLIGHT; Frame++) {
        vkDestroyFence(VulkanState.Device, VulkanState.SwapchainFences[Frame], NULL);
        vkDestroyImageView(VulkanState.Device, VulkanState.SwapchainImageViews[Frame], NULL);
    }
    
    vkDestroySemaphore(VulkanState.Device, VulkanState.RenderedSemaphore, NULL);
    vkDestroySemaphore(VulkanState.Device, VulkanState.AvailableSemaphore, NULL);
    vkDestroyFence(VulkanState.Device, VulkanState.UploadFence, NULL);
    vkDestroyFence(VulkanState.Device, VulkanState.ComputeFence, NULL);
    vkDestroySwapchainKHR(VulkanState.Device, VulkanState.Swapchain, NULL);
    vkDestroyDevice(VulkanState.Device, NULL);
    vkDestroySurfaceKHR(VulkanState.Instance, VulkanState.Surface, NULL);
    vkDestroyInstance(VulkanState.Instance, NULL);
}

void CheckVk(VkResult Result, const char* Message)
{
    if (Result != VK_SUCCESS)
        printf("%s", Message);
}