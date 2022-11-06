/*
** EPITECH PROJECT 2022
** elara_gpu_pipeline.c
** File description:
** GPU pipeline implementation
*/

#include "elara_vulkan.h"
#include "elara_spirv_reflect.h"

u32 GetFormatSize(VkFormat Format);

void PipelineInitGraphics(gpu_pipeline* Pipeline, gpu_pipeline_descriptor* Descriptor)
{
    Pipeline->BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    Pipeline->PipelineType = GpuPipelineType_Graphics;
    
    VkPipelineShaderStageCreateInfo PipelineShaderStages[8];
    memset(PipelineShaderStages, 0, sizeof(PipelineShaderStages));
    i32 PipelineStageCount = 0;
    
    // Used to reflect the input layout from the shader
    SpvReflectShaderModule InputLayoutReflect;
    
    // Setup shader stages
    if (Descriptor->UseMS)
    {
        PipelineShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        PipelineShaderStages[0].stage = VK_SHADER_STAGE_TASK_BIT_EXT;
        PipelineShaderStages[0].module = Descriptor->ShaderInfo.Task->Shader;
        PipelineShaderStages[0].pName = "main";
        
        PipelineShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        PipelineShaderStages[1].stage = VK_SHADER_STAGE_MESH_BIT_EXT;
        PipelineShaderStages[1].module = Descriptor->ShaderInfo.Mesh->Shader;
        PipelineShaderStages[1].pName = "main";
        
        PipelineShaderStages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        PipelineShaderStages[2].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        PipelineShaderStages[2].module = Descriptor->ShaderInfo.Fragment->Shader;
        PipelineShaderStages[2].pName = "main";
        
        PipelineStageCount = 3;
    } 
    else
    {
        SpvReflectResult Result = spvReflectCreateShaderModule(Descriptor->ShaderInfo.Vertex->ByteCodeSize, Descriptor->ShaderInfo.Vertex->ByteCode, &InputLayoutReflect);
        assert(Result == SPV_REFLECT_RESULT_SUCCESS);
        
        PipelineShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        PipelineShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        PipelineShaderStages[0].module = Descriptor->ShaderInfo.Vertex->Shader;
        PipelineShaderStages[0].pName = "main";
        
        PipelineShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        PipelineShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        PipelineShaderStages[1].module = Descriptor->ShaderInfo.Fragment->Shader;
        PipelineShaderStages[1].pName = "main";
        
        PipelineStageCount = 2;
    }
    
    // Reflect input layout
    VkVertexInputBindingDescription VertexInputBindingDesc = {0};
    VertexInputBindingDesc.binding = 0;
    VertexInputBindingDesc.stride = 0;
    VertexInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    VkPipelineVertexInputStateCreateInfo VertexInputStateInfo = {0};
    VkPipelineInputAssemblyStateCreateInfo InputAssembly = {0};
    VkVertexInputAttributeDescription* AttributeDescriptions = NULL;
    SpvReflectInterfaceVariable** InputVars = NULL;
    
    if (Descriptor->ReflectInputLayout)
    {
        u32 InputVarCount = 0;
        spvReflectEnumerateInputVariables(&InputLayoutReflect, &InputVarCount, 0);
        InputVars = calloc(InputVarCount, sizeof(SpvReflectInterfaceVariable*));
        spvReflectEnumerateInputVariables(&InputLayoutReflect, &InputVarCount, InputVars);
        
        AttributeDescriptions = malloc(InputVarCount * sizeof(VkVertexInputAttributeDescription));
        
        for (i32 Location = 0; Location < InputVarCount; Location++)
        {
            for (i32 Index = 0; Index < InputVarCount; Index++)
            {
                SpvReflectInterfaceVariable* Variable = InputVars[Index];
                
                if (Variable->location == (u32)Location)
                {
                    VkVertexInputAttributeDescription Attribute = {0};
                    Attribute.location = Variable->location;
                    Attribute.binding = 0;
                    Attribute.format = (VkFormat)Variable->format;
                    Attribute.offset = VertexInputBindingDesc.stride;
                    
                    AttributeDescriptions[Index] = Attribute;
                    VertexInputBindingDesc.stride += GetFormatSize(Attribute.format);
                }
            }
        }
        
        VertexInputStateInfo.vertexBindingDescriptionCount = VertexInputBindingDesc.stride == 0 ? 0 : 1;
        VertexInputStateInfo.pVertexBindingDescriptions = &VertexInputBindingDesc;
        VertexInputStateInfo.vertexAttributeDescriptionCount = InputVarCount;
        VertexInputStateInfo.pVertexAttributeDescriptions = AttributeDescriptions;
        
        free(InputVars);
    }
    else
    {
        VertexInputStateInfo.vertexBindingDescriptionCount = 0;
        VertexInputStateInfo.pVertexBindingDescriptions = NULL;
        VertexInputStateInfo.vertexAttributeDescriptionCount = 0;
        VertexInputStateInfo.pVertexAttributeDescriptions = NULL;
    }
    
    InputAssembly.topology = Descriptor->PrimitiveTopology;
    InputAssembly.primitiveRestartEnable = VK_FALSE;
    
    // Setup dynamic states and views
    VkDynamicState States[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    
    VkPipelineDynamicStateCreateInfo DynamicState = {0};
    DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicState.dynamicStateCount = 2;
    DynamicState.pDynamicStates = States;
    
    VkViewport Viewport = {0};
    Viewport.width = (f32)PlatformState.Dimensions.Width;
    Viewport.height = (f32)PlatformState.Dimensions.Height;
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;
    
    VkRect2D Scissor = {0};
    Scissor.offset.x = 0;
    Scissor.offset.y = 0;
    Scissor.extent = VulkanState.SwapchainExtent;
    
    VkPipelineViewportStateCreateInfo ViewportState = {0};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = 1;
    ViewportState.pViewports = &Viewport;
    ViewportState.scissorCount = 1;
    ViewportState.pScissors = &Scissor;
    
    // Rendering info
    VkPipelineRasterizationStateCreateInfo Rasterizer = {0};
    Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Rasterizer.depthClampEnable = VK_FALSE;
    Rasterizer.rasterizerDiscardEnable = VK_FALSE;
    Rasterizer.polygonMode = Descriptor->PolygonMode;
    Rasterizer.lineWidth = 1.0f;
    Rasterizer.cullMode = Descriptor->CullMode;
    Rasterizer.frontFace = Descriptor->FrontFace;
    Rasterizer.depthBiasEnable = false;
    
    VkPipelineMultisampleStateCreateInfo Multisampling = {0};
    Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Multisampling.sampleShadingEnable = VK_FALSE;
    Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    
    VkPipelineColorBlendAttachmentState ColorBlends[32] = {0};
    for (i32 ColorBlend = 0; ColorBlend < Descriptor->ColorAttachmentCount; ColorBlend++)
    {
        ColorBlends[ColorBlend].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        ColorBlends[ColorBlend].blendEnable = VK_FALSE;
    }
    
    VkPipelineColorBlendStateCreateInfo ColorBlending = {0};
    ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlending.logicOpEnable = VK_FALSE;
    ColorBlending.logicOp = VK_LOGIC_OP_COPY;
    ColorBlending.attachmentCount = Descriptor->ColorAttachmentCount;
    ColorBlending.pAttachments = ColorBlends;
    ColorBlending.blendConstants[0] = 0.0f;
    ColorBlending.blendConstants[1] = 0.0f;
    ColorBlending.blendConstants[2] = 0.0f;
    ColorBlending.blendConstants[3] = 0.0f;
    
    VkPipelineDepthStencilStateCreateInfo DepthStencil = {0};
    DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    DepthStencil.depthTestEnable = VK_TRUE;
    DepthStencil.depthWriteEnable = VK_TRUE;
    DepthStencil.depthCompareOp = Descriptor->DepthCompare;
    DepthStencil.depthBoundsTestEnable = VK_FALSE;
    DepthStencil.minDepthBounds = 0.0f;
    DepthStencil.maxDepthBounds = 1.0f;
    DepthStencil.stencilTestEnable = VK_FALSE;
    
    VkPipelineLayoutCreateInfo LayoutInfo = {0};
    LayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    
    if (Descriptor->ShaderInfo.SetLayoutCount > 0)
    {
        VkDescriptorSetLayout Layouts[16];
        for (i32 Layout = 0; Layout < Descriptor->ShaderInfo.SetLayoutCount; Layout++)
            Layouts[Layout] = Descriptor->ShaderInfo.Layouts[Layout].Layout;
        
        LayoutInfo.setLayoutCount = Descriptor->ShaderInfo.SetLayoutCount;
        LayoutInfo.pSetLayouts = Layouts;
    }
    
    if (Descriptor->ShaderInfo.PushConstantSize > 0)
    {
        VkPushConstantRange Range = {0};
        Range.offset = 0;
        Range.size = Descriptor->ShaderInfo.PushConstantSize;
        Range.stageFlags = VK_SHADER_STAGE_ALL;
        
        LayoutInfo.pushConstantRangeCount = 1;
        LayoutInfo.pPushConstantRanges = &Range;
    }
    
    VkResult Result = vkCreatePipelineLayout(VulkanState.Device, &LayoutInfo, NULL, &Pipeline->PipelineLayout);
    CheckVk(Result, "Failed to create graphics pipeline layout!");
    
    VkPipelineRenderingCreateInfo RenderingCreateInfo = {0};
    RenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    RenderingCreateInfo.colorAttachmentCount = Descriptor->ColorAttachmentCount;
    RenderingCreateInfo.depthAttachmentFormat = Descriptor->DepthFormat;
    RenderingCreateInfo.stencilAttachmentFormat = Descriptor->DepthFormat;
    RenderingCreateInfo.pColorAttachmentFormats = Descriptor->ColorFormats;
    
    VkGraphicsPipelineCreateInfo CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    CreateInfo.stageCount = PipelineStageCount;
    CreateInfo.pStages = PipelineShaderStages;
    CreateInfo.pViewportState = &ViewportState;
    CreateInfo.pRasterizationState = &Rasterizer;
    CreateInfo.pMultisampleState = &Multisampling;
    CreateInfo.pColorBlendState = &ColorBlending;
    CreateInfo.layout = Pipeline->PipelineLayout;
    CreateInfo.renderPass = VK_NULL_HANDLE;
    CreateInfo.pDynamicState = &DynamicState;
    CreateInfo.pDepthStencilState = &DepthStencil;
    CreateInfo.subpass = 0;
    CreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    CreateInfo.pNext = &RenderingCreateInfo;
    
    Result = vkCreateGraphicsPipelines(VulkanState.Device, VK_NULL_HANDLE, 1, &CreateInfo, NULL, &Pipeline->Pipeline);
    CheckVk(Result, "Failed to create graphics pipeline!");
    
    if (AttributeDescriptions)
        free(AttributeDescriptions);
}

void PipelineInitCompute(gpu_pipeline* Pipeline, gpu_pipeline_descriptor* Descriptor)
{
    Pipeline->BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    Pipeline->PipelineType = GpuPipelineType_Compute;
    
    VkPipelineLayoutCreateInfo LayoutInfo = {0};
    LayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    
    if (Descriptor->ShaderInfo.SetLayoutCount > 0)
    {
        VkDescriptorSetLayout Layouts[16];
        for (i32 Layout = 0; Layout < Descriptor->ShaderInfo.SetLayoutCount; Layout++)
            Layouts[Layout] = Descriptor->ShaderInfo.Layouts[Layout].Layout;
        
        LayoutInfo.setLayoutCount = Descriptor->ShaderInfo.SetLayoutCount;
        LayoutInfo.pSetLayouts = Layouts;
    }
    
    if (Descriptor->ShaderInfo.PushConstantSize > 0)
    {
        VkPushConstantRange Range = {0};
        Range.offset = 0;
        Range.size = Descriptor->ShaderInfo.PushConstantSize;
        Range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        
        LayoutInfo.pushConstantRangeCount = 1;
        LayoutInfo.pPushConstantRanges = &Range;
    }
    
    VkResult Result = vkCreatePipelineLayout(VulkanState.Device, &LayoutInfo, NULL, &Pipeline->PipelineLayout);
    CheckVk(Result, "Failed to create compute pipeline layout!");
    
    VkComputePipelineCreateInfo Info = {0};
    Info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    Info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    Info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    Info.stage.module = Descriptor->ShaderInfo.Compute->Shader;
    Info.stage.pName = "main";
    Info.layout = Pipeline->PipelineLayout;
    
    Result = vkCreateComputePipelines(VulkanState.Device, VK_NULL_HANDLE, 1, &Info, NULL, &Pipeline->Pipeline);
    CheckVk(Result, "Failed to create compute pipeline!");
}

void PipelineFree(gpu_pipeline* Pipeline)
{
    vkDestroyPipeline(VulkanState.Device, Pipeline->Pipeline, NULL);
    vkDestroyPipelineLayout(VulkanState.Device, Pipeline->PipelineLayout, NULL);
}

u32 GetFormatSize(VkFormat Format)
{
    switch (Format) {
        case VK_FORMAT_UNDEFINED: return 0;
        case VK_FORMAT_R4G4_UNORM_PACK8: return 1;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return 2;
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return 2;
        case VK_FORMAT_R5G6B5_UNORM_PACK16: return 2;
        case VK_FORMAT_B5G6R5_UNORM_PACK16: return 2;
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return 2;
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return 2;
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return 2;
        case VK_FORMAT_R8_UNORM: return 1;
        case VK_FORMAT_R8_SNORM: return 1;
        case VK_FORMAT_R8_USCALED: return 1;
        case VK_FORMAT_R8_SSCALED: return 1;
        case VK_FORMAT_R8_UINT: return 1;
        case VK_FORMAT_R8_SINT: return 1;
        case VK_FORMAT_R8_SRGB: return 1;
        case VK_FORMAT_R8G8_UNORM: return 2;
        case VK_FORMAT_R8G8_SNORM: return 2;
        case VK_FORMAT_R8G8_USCALED: return 2;
        case VK_FORMAT_R8G8_SSCALED: return 2;
        case VK_FORMAT_R8G8_UINT: return 2;
        case VK_FORMAT_R8G8_SINT: return 2;
        case VK_FORMAT_R8G8_SRGB: return 2;
        case VK_FORMAT_R8G8B8_UNORM: return 3;
        case VK_FORMAT_R8G8B8_SNORM: return 3;
        case VK_FORMAT_R8G8B8_USCALED: return 3;
        case VK_FORMAT_R8G8B8_SSCALED: return 3;
        case VK_FORMAT_R8G8B8_UINT: return 3;
        case VK_FORMAT_R8G8B8_SINT: return 3;
        case VK_FORMAT_R8G8B8_SRGB: return 3;
        case VK_FORMAT_B8G8R8_UNORM: return 3;
        case VK_FORMAT_B8G8R8_SNORM: return 3;
        case VK_FORMAT_B8G8R8_USCALED: return 3;
        case VK_FORMAT_B8G8R8_SSCALED: return 3;
        case VK_FORMAT_B8G8R8_UINT: return 3;
        case VK_FORMAT_B8G8R8_SINT: return 3;
        case VK_FORMAT_B8G8R8_SRGB: return 3;
        case VK_FORMAT_R8G8B8A8_UNORM: return 4;
        case VK_FORMAT_R8G8B8A8_SNORM: return 4;
        case VK_FORMAT_R8G8B8A8_USCALED: return 4;
        case VK_FORMAT_R8G8B8A8_SSCALED: return 4;
        case VK_FORMAT_R8G8B8A8_UINT: return 4;
        case VK_FORMAT_R8G8B8A8_SINT: return 4;
        case VK_FORMAT_R8G8B8A8_SRGB: return 4;
        case VK_FORMAT_B8G8R8A8_UNORM: return 4;
        case VK_FORMAT_B8G8R8A8_SNORM: return 4;
        case VK_FORMAT_B8G8R8A8_USCALED: return 4;
        case VK_FORMAT_B8G8R8A8_SSCALED: return 4;
        case VK_FORMAT_B8G8R8A8_UINT: return 4;
        case VK_FORMAT_B8G8R8A8_SINT: return 4;
        case VK_FORMAT_B8G8R8A8_SRGB: return 4;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return 4;
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return 4;
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32: return 4;
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: return 4;
        case VK_FORMAT_A8B8G8R8_UINT_PACK32: return 4;
        case VK_FORMAT_A8B8G8R8_SINT_PACK32: return 4;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return 4;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return 4;
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32: return 4;
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32: return 4;
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: return 4;
        case VK_FORMAT_A2R10G10B10_UINT_PACK32: return 4;
        case VK_FORMAT_A2R10G10B10_SINT_PACK32: return 4;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return 4;
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32: return 4;
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32: return 4;
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: return 4;
        case VK_FORMAT_A2B10G10R10_UINT_PACK32: return 4;
        case VK_FORMAT_A2B10G10R10_SINT_PACK32: return 4;
        case VK_FORMAT_R16_UNORM: return 2;
        case VK_FORMAT_R16_SNORM: return 2;
        case VK_FORMAT_R16_USCALED: return 2;
        case VK_FORMAT_R16_SSCALED: return 2;
        case VK_FORMAT_R16_UINT: return 2;
        case VK_FORMAT_R16_SINT: return 2;
        case VK_FORMAT_R16_SFLOAT: return 2;
        case VK_FORMAT_R16G16_UNORM: return 4;
        case VK_FORMAT_R16G16_SNORM: return 4;
        case VK_FORMAT_R16G16_USCALED: return 4;
        case VK_FORMAT_R16G16_SSCALED: return 4;
        case VK_FORMAT_R16G16_UINT: return 4;
        case VK_FORMAT_R16G16_SINT: return 4;
        case VK_FORMAT_R16G16_SFLOAT: return 4;
        case VK_FORMAT_R16G16B16_UNORM: return 6;
        case VK_FORMAT_R16G16B16_SNORM: return 6;
        case VK_FORMAT_R16G16B16_USCALED: return 6;
        case VK_FORMAT_R16G16B16_SSCALED: return 6;
        case VK_FORMAT_R16G16B16_UINT: return 6;
        case VK_FORMAT_R16G16B16_SINT: return 6;
        case VK_FORMAT_R16G16B16_SFLOAT: return 6;
        case VK_FORMAT_R16G16B16A16_UNORM: return 8;
        case VK_FORMAT_R16G16B16A16_SNORM: return 8;
        case VK_FORMAT_R16G16B16A16_USCALED: return 8;
        case VK_FORMAT_R16G16B16A16_SSCALED: return 8;
        case VK_FORMAT_R16G16B16A16_UINT: return 8;
        case VK_FORMAT_R16G16B16A16_SINT: return 8;
        case VK_FORMAT_R16G16B16A16_SFLOAT: return 8;
        case VK_FORMAT_R32_UINT: return 4;
        case VK_FORMAT_R32_SINT: return 4;
        case VK_FORMAT_R32_SFLOAT: return 4;
        case VK_FORMAT_R32G32_UINT: return 8;
        case VK_FORMAT_R32G32_SINT: return 8;
        case VK_FORMAT_R32G32_SFLOAT: return 8;
        case VK_FORMAT_R32G32B32_UINT: return 12;
        case VK_FORMAT_R32G32B32_SINT: return 12;
        case VK_FORMAT_R32G32B32_SFLOAT: return 12;
        case VK_FORMAT_R32G32B32A32_UINT: return 16;
        case VK_FORMAT_R32G32B32A32_SINT: return 16;
        case VK_FORMAT_R32G32B32A32_SFLOAT: return 16;
        case VK_FORMAT_R64_UINT: return 8;
        case VK_FORMAT_R64_SINT: return 8;
        case VK_FORMAT_R64_SFLOAT: return 8;
        case VK_FORMAT_R64G64_UINT: return 16;
        case VK_FORMAT_R64G64_SINT: return 16;
        case VK_FORMAT_R64G64_SFLOAT: return 16;
        case VK_FORMAT_R64G64B64_UINT: return 24;
        case VK_FORMAT_R64G64B64_SINT: return 24;
        case VK_FORMAT_R64G64B64_SFLOAT: return 24;
        case VK_FORMAT_R64G64B64A64_UINT: return 32;
        case VK_FORMAT_R64G64B64A64_SINT: return 32;
        case VK_FORMAT_R64G64B64A64_SFLOAT: return 32;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return 4;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return 4;
    }
    
    return 0;
}