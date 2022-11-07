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
    
    VulkanState.Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	vkGetPhysicalDeviceFeatures2(VulkanState.GPU, &VulkanState.Features);
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
    Features.multiDrawIndirect = 1;
    
    VkPhysicalDeviceDescriptorIndexingFeatures DIFeatures;
    memset(&DIFeatures, 0, sizeof(DIFeatures));
    DIFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    DIFeatures.descriptorBindingPartiallyBound = 1;
    
    VkPhysicalDeviceMeshShaderFeaturesEXT MSFeatures;
    memset(&MSFeatures, 0, sizeof(MSFeatures));
    MSFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
    MSFeatures.taskShader = VK_TRUE;
    MSFeatures.meshShader = VK_TRUE;
    MSFeatures.meshShaderQueries = VK_TRUE;
    MSFeatures.pNext = &DIFeatures;
    
    VkPhysicalDeviceBufferDeviceAddressFeatures BDAFeatures;
    memset(&BDAFeatures, 0, sizeof(BDAFeatures));
    BDAFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    BDAFeatures.bufferDeviceAddress = VK_TRUE;
    BDAFeatures.bufferDeviceAddressCaptureReplay = VK_TRUE;
    BDAFeatures.pNext = &MSFeatures;
    
    VulkanState.Features.features = Features;
    VulkanState.Features.pNext = &BDAFeatures;
    
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
        
        if (!strcmp(VK_EXT_MESH_SHADER_EXTENSION_NAME, Property.extensionName))
        {
            VulkanState.DeviceExtensions[VulkanState.DeviceExtensionCount++] = VK_EXT_MESH_SHADER_EXTENSION_NAME;
        }
        
        if (!strcmp(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, Property.extensionName))
        {
            VulkanState.DeviceExtensions[VulkanState.DeviceExtensionCount++] = VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME;
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
    CreateInfo.pNext = &VulkanState.Features;
    
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
        
        VulkanState.SwapchainImageHandles[ImageIterator].Extent = VulkanState.SwapchainExtent;
        VulkanState.SwapchainImageHandles[ImageIterator].Format = VulkanState.SwapchainFormat;
        VulkanState.SwapchainImageHandles[ImageIterator].Image = Image;
        VulkanState.SwapchainImageHandles[ImageIterator].ImageView = VulkanState.SwapchainImageViews[ImageIterator];
        VulkanState.SwapchainImageHandles[ImageIterator].Layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VulkanState.SwapchainImageHandles[ImageIterator].MipLevels = 1;
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
    
    for (int Frame = 0; Frame < FRAMES_IN_FLIGHT; Frame++)
    {
        CommandBufferInit(&VulkanState.SwapchainCommandBuffers[Frame], CommandBufferType_Graphics);
    }
}

void InitAllocator()
{
    VmaAllocatorCreateInfo CreateInfo = {0};
    CreateInfo.device = VulkanState.Device;
    CreateInfo.instance = VulkanState.Instance;
    CreateInfo.physicalDevice = VulkanState.GPU;
    CreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    CreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    
    VkResult Result = vmaCreateAllocator(&CreateInfo, &VulkanState.Allocator);
    CheckVk(Result, "Failed to create VMA allocator!");
}

void InitPool()
{
    VkDescriptorPoolSize Sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 4096 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4096 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4096 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4096 }
    };
    
    VkDescriptorPoolCreateInfo CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    CreateInfo.pPoolSizes = Sizes;
    CreateInfo.poolSizeCount = 4;
    CreateInfo.maxSets = 2048;
    CreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    
    VkResult Result = vkCreateDescriptorPool(VulkanState.Device, &CreateInfo, NULL, &VulkanState.DescriptorPool);
    CheckVk(Result, "Failed to create descriptor pool!");
}

void InitVulkan()
{
    memset(&VulkanState, 0, sizeof(vulkan_state));
    
    InitInstance();
    InitPhysicalDevice();
    PlatformCreateSurface(VulkanState.Instance, &VulkanState.Surface);
    InitDevice();
    InitSwapchain();
    InitSync();
    InitCommand();
    InitAllocator();
    InitPool();
}

void ExitVulkan()
{    
    vkDeviceWaitIdle(VulkanState.Device);
    
    vmaDestroyAllocator(VulkanState.Allocator);
    
    vkDestroyDescriptorPool(VulkanState.Device, VulkanState.DescriptorPool, NULL);
    vkDestroyCommandPool(VulkanState.Device, VulkanState.ComputePool, NULL);
    vkDestroyCommandPool(VulkanState.Device, VulkanState.UploadPool, NULL);
    vkDestroyCommandPool(VulkanState.Device, VulkanState.GraphicsPool, NULL);
    
    for (u32 Frame = 0; Frame < FRAMES_IN_FLIGHT; Frame++) 
    {
        vkDestroyFence(VulkanState.Device, VulkanState.SwapchainFences[Frame], NULL);
        vkDestroyImageView(VulkanState.Device, VulkanState.SwapchainImageViews[Frame], NULL);
    }
    free(VulkanState.SwapchainImages);
    
    vkDestroySemaphore(VulkanState.Device, VulkanState.RenderedSemaphore, NULL);
    vkDestroySemaphore(VulkanState.Device, VulkanState.AvailableSemaphore, NULL);
    vkDestroyFence(VulkanState.Device, VulkanState.UploadFence, NULL);
    vkDestroyFence(VulkanState.Device, VulkanState.ComputeFence, NULL);
    vkDestroySwapchainKHR(VulkanState.Device, VulkanState.Swapchain, NULL);
    vkDestroyDevice(VulkanState.Device, NULL);
    vkDestroySurfaceKHR(VulkanState.Instance, VulkanState.Surface, NULL);
    vkDestroyInstance(VulkanState.Instance, NULL);
}

void ResizeVulkan()
{
    if (VulkanState.Swapchain != VK_NULL_HANDLE)
    {   
        vkDeviceWaitIdle(VulkanState.Device);
        
        for (u32 Frame = 0; Frame < FRAMES_IN_FLIGHT; Frame++) 
        {
            vkDestroyImageView(VulkanState.Device, VulkanState.SwapchainImageViews[Frame], NULL);
        }
        free(VulkanState.SwapchainImages);
        vkDestroySwapchainKHR(VulkanState.Device, VulkanState.Swapchain, NULL);
        
        InitSwapchain();
    }
}

void RenderBegin()
{
    vkAcquireNextImageKHR(VulkanState.Device, VulkanState.Swapchain, UINT32_MAX, VulkanState.AvailableSemaphore, VK_NULL_HANDLE, (u32*)&VulkanState.ImageIndex);
    
    command_buffer* Command = &VulkanState.SwapchainCommandBuffers[VulkanState.ImageIndex];
    gpu_image* SwapchainImage = &VulkanState.SwapchainImageHandles[VulkanState.ImageIndex];
    
    vkWaitForFences(VulkanState.Device, 1, &VulkanState.SwapchainFences[VulkanState.ImageIndex], VK_TRUE, UINT32_MAX);
    vkResetFences(VulkanState.Device, 1, &VulkanState.SwapchainFences[VulkanState.ImageIndex]);
    vkResetCommandBuffer(Command->Handle, 0);
    
    CommandBufferBegin(Command);
    CommandBufferTransitionImageLayout(Command, SwapchainImage, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0);
}

void RenderEnd()
{
    command_buffer* Command = &VulkanState.SwapchainCommandBuffers[VulkanState.ImageIndex];
    gpu_image* SwapchainImage = &VulkanState.SwapchainImageHandles[VulkanState.ImageIndex];
    
    CommandBufferTransitionImageLayout(Command, SwapchainImage, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0);
    CommandBufferEnd(Command);
    
    VkSemaphore WaitSemaphores[] = { VulkanState.AvailableSemaphore };
    VkSemaphore SignalSemaphores[] = { VulkanState.RenderedSemaphore };
    VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    
    VkSubmitInfo SubmitInfo = {0};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = WaitSemaphores;
    SubmitInfo.pWaitDstStageMask = WaitStages;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &Command->Handle;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = SignalSemaphores;
    
    vkResetFences(VulkanState.Device, 1, &VulkanState.SwapchainFences[VulkanState.ImageIndex]);
    
    VkResult Result = vkQueueSubmit(VulkanState.GraphicsQueue, 1, &SubmitInfo, VulkanState.SwapchainFences[VulkanState.ImageIndex]);
    CheckVk(Result, "Failed to submit frame command buffers!");
}

void RenderPresent()
{
    VkSemaphore WaitSemaphores[] = { VulkanState.RenderedSemaphore };
    VkSwapchainKHR Swapchains[] = { VulkanState.Swapchain };
    
    VkPresentInfoKHR PresentInfo = {0};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = WaitSemaphores;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = Swapchains;
    PresentInfo.pImageIndices = (u32*)&VulkanState.ImageIndex;
    
    vkQueuePresentKHR(VulkanState.GraphicsQueue, &PresentInfo);
}

void CheckVk(VkResult Result, const char* Message)
{
    if (Result != VK_SUCCESS)
    {
        printf("%s", Message);
    }
}