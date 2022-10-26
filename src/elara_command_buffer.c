/*
** EPITECH PROJECT, 2022
** elara_command_buffer.c
** File description:
** Command buffer implementation
*/

#include "elara_vulkan.h"

VkCommandPool GetCommandPoolFromType(command_buffer_type Type)
{
    if (Type == CommandBufferType_Graphics)
        return VulkanState.GraphicsPool;
    if (Type == CommandBufferType_Compute)
        return VulkanState.ComputePool;
    if (Type == CommandBufferType_Upload)
        return VulkanState.UploadPool;
    return VulkanState.GraphicsPool;
}

void CommandBufferInit(command_buffer* CommandBuffer, command_buffer_type Type)
{
    CommandBuffer->CommandBufferType = Type;
    
    VkCommandBufferAllocateInfo AllocInfo = {0};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.commandPool = GetCommandPoolFromType(Type);
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandBufferCount = 1;
    
    VkResult Result = vkAllocateCommandBuffers(VulkanState.Device, &AllocInfo, &CommandBuffer->Handle);
    CheckVk(Result, "Failed to allocate command buffer!");
}

void CommandBufferFree(command_buffer* CommandBuffer)
{
    vkFreeCommandBuffers(VulkanState.Device, GetCommandPoolFromType(CommandBuffer->CommandBufferType), 1, &CommandBuffer->Handle);
}

void CommandBufferBegin(command_buffer* CommandBuffer)
{
    VkCommandBufferBeginInfo BeginInfo = {0};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    VkResult Result = vkBeginCommandBuffer(CommandBuffer->Handle, &BeginInfo);
    CheckVk(Result, "Failed to begin command buffer!");
}

void CommandBufferEnd(command_buffer* CommandBuffer)
{
    CheckVk(vkEndCommandBuffer(CommandBuffer->Handle), "Failed to end command buffer!");
}

void CommandBufferSubmit(command_buffer* CommandBuffer)
{
    VkSubmitInfo SubmitInfo = {0};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer->Handle;
    
    switch (CommandBuffer->CommandBufferType)
    {
        case CommandBufferType_Graphics:
        {
            vkQueueSubmit(VulkanState.GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(VulkanState.GraphicsQueue);
            break;
        }
        case CommandBufferType_Compute:
        {
            vkQueueSubmit(VulkanState.ComputeQueue, 1, &SubmitInfo, VulkanState.ComputeFence);
            vkWaitForFences(VulkanState.Device, 1, &VulkanState.ComputeFence, VK_TRUE, UINT32_MAX);
            vkResetFences(VulkanState.Device, 1, &VulkanState.ComputeFence);
            vkResetCommandPool(VulkanState.Device, VulkanState.ComputePool, 0);
        	break;
        }
        case CommandBufferType_Upload:
        {
            vkQueueSubmit(VulkanState.GraphicsQueue, 1, &SubmitInfo, VulkanState.UploadFence);
            vkWaitForFences(VulkanState.Device, 1, &VulkanState.UploadFence, VK_TRUE, UINT32_MAX);
            vkResetFences(VulkanState.Device, 1, &VulkanState.UploadFence);
            vkResetCommandPool(VulkanState.Device, VulkanState.UploadPool, 0);
            break;
        }
    }
}