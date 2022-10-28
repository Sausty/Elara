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

void CommandBufferBeginDynamicRendering(command_buffer* CommandBuffer, dynamic_rendering_info* Info)
{
    u32 ColorIterator = Info->HasDepth ? Info->ImageCount - 1 : Info->ImageCount;
    
    VkRect2D RenderArea = {0};
    RenderArea.extent.width = (u32)Info->Width;
    RenderArea.extent.height = (u32)Info->Height;
    RenderArea.offset.x = 0;
    RenderArea.offset.y = 0;
    
    VkRenderingInfo RenderingInfo = {0};
    RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    RenderingInfo.renderArea = RenderArea;
    RenderingInfo.colorAttachmentCount = ColorIterator;
    RenderingInfo.layerCount = 1;
    
    VkRenderingAttachmentInfo ColorAttachments[64] = {0};
    
    for (u32 Iterator = 0; Iterator < ColorIterator; Iterator++)
    {
        gpu_image* Image = Info->Images[Iterator];
        
        VkClearValue ClearValue = {0};
        ClearValue.color.float32[0] = Info->R;
        ClearValue.color.float32[1] = Info->G;
        ClearValue.color.float32[2] = Info->B;
        ClearValue.color.float32[3] = Info->A;
        
        VkRenderingAttachmentInfo ColorAttachmentInfo = {0};
        ColorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        ColorAttachmentInfo.imageView = Image->ImageView;
        ColorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        ColorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
        ColorAttachmentInfo.loadOp = Info->ReadColor ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
        ColorAttachmentInfo.storeOp = Info->ReadColor ? VK_ATTACHMENT_STORE_OP_NONE : VK_ATTACHMENT_STORE_OP_STORE;
        ColorAttachmentInfo.clearValue = ClearValue;
        
        ColorAttachments[Iterator] = ColorAttachmentInfo;
    }
    
    if (Info->HasDepth)
    {
        gpu_image* Image = Info->Images[ColorIterator];
        
        VkClearValue ClearValue = {0};
        ClearValue.depthStencil.depth = Info->ReadDepth ? 0.0f : 1.0f;
        ClearValue.depthStencil.stencil = 0.0f;
        
        VkRenderingAttachmentInfo DepthAttachmentInfo = {0};
        DepthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        DepthAttachmentInfo.imageView = Image->ImageView;
        DepthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        DepthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
        DepthAttachmentInfo.loadOp = Info->ReadDepth ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
        DepthAttachmentInfo.storeOp = Info->ReadDepth ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
        DepthAttachmentInfo.clearValue = ClearValue;
        
        RenderingInfo.pStencilAttachment = &DepthAttachmentInfo;
        RenderingInfo.pDepthAttachment = &DepthAttachmentInfo;
    }
    
    RenderingInfo.pColorAttachments = ColorAttachments;
    
    vkCmdBeginRendering(CommandBuffer->Handle, &RenderingInfo);
}

void CommandBufferEndDynamicRendering(command_buffer* CommandBuffer)
{
    vkCmdEndRendering(CommandBuffer->Handle);
}