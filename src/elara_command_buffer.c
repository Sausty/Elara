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
        return(VulkanState.GraphicsPool);
    if (Type == CommandBufferType_Compute)
        return(VulkanState.ComputePool);
    if (Type == CommandBufferType_Upload)
        return(VulkanState.UploadPool);
    return(VulkanState.GraphicsPool);
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

void CommandBufferSetViewport(command_buffer* CommandBuffer, i32 Width, i32 Height)
{
    VkViewport Viewport = {0};
    Viewport.width = Width;
    Viewport.height = Height;
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;
    
    VkRect2D Scissor = {0};
    Scissor.offset.x = 0;
    Scissor.offset.y = 0;
    Scissor.extent.width = Width;
    Scissor.extent.height = Height;
    
    vkCmdSetViewport(CommandBuffer->Handle, 0, 1, &Viewport);
    vkCmdSetScissor(CommandBuffer->Handle, 0, 1, &Scissor);
}

void CommandBufferSetVertexBuffer(command_buffer* CommandBuffer, gpu_buffer* Buffer)
{
    VkBuffer Buffers[] = { Buffer->Buffer };
    VkDeviceSize Offsets[] = { 0 };
    
    vkCmdBindVertexBuffers(CommandBuffer->Handle, 0, 1, Buffers, Offsets);
}

void CommandBufferSetIndexBuffer(command_buffer* CommandBuffer, gpu_buffer* Buffer)
{	
    vkCmdBindIndexBuffer(CommandBuffer->Handle, Buffer->Buffer, 0, VK_INDEX_TYPE_UINT32);
}

void CommandBufferTransitionImageLayout(command_buffer* CommandBuffer, gpu_image* Image, u32 SrcA, u32 DstA, u32 SrcL, u32 DstL, u32 SrcS, u32 DstS, u32 Layer)
{
    VkImageSubresourceRange Range = { 0 };
    Range.baseMipLevel = 0;
    Range.levelCount = Image->MipLevels == 1 ? VK_REMAINING_MIP_LEVELS : Image->MipLevels;
    Range.baseArrayLayer = Layer;
    Range.layerCount = VK_REMAINING_ARRAY_LAYERS;
    Range.aspectMask = (VkImageAspectFlagBits)GetImageAspect(Image->Format);
    
    VkImageMemoryBarrier Barrier = { 0 };
    Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    Barrier.srcAccessMask = SrcA;
    Barrier.dstAccessMask = DstA;
    Barrier.oldLayout = SrcL;
    Barrier.newLayout = DstL;
    Barrier.image = Image->Image;
    Barrier.subresourceRange = Range;
    
    vkCmdPipelineBarrier(CommandBuffer->Handle, SrcS, DstS, 0, 0, NULL, 0, NULL, 1, &Barrier);
}