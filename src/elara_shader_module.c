/*
** EPITECH PROJECT 2022
** elara_shader_module.c
** File description:
** Shader module implementation
*/

#include "elara_vulkan.h"
#include "elara_platform.h"

void ShaderLoad(shader_module* Shader, const char* Path)
{
    Shader->ByteCode = (u32*)PlatformReadFile(Path, &Shader->ByteCodeSize);
    
    VkShaderModuleCreateInfo CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreateInfo.codeSize = Shader->ByteCodeSize;
    CreateInfo.pCode = Shader->ByteCode;
    
    VkResult Result = vkCreateShaderModule(VulkanState.Device, &CreateInfo, NULL, &Shader->Shader);
    CheckVk(Result, "Failed to create shader module!");
    
    free(Shader->ByteCode);
}

void ShaderFree(shader_module* Shader)
{
    vkDestroyShaderModule(VulkanState.Device, Shader->Shader, NULL);
}