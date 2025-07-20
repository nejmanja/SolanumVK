#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <exception>

#include <vulkan/vulkan.h>

class ShaderLoader
{
public:
    static VkShaderModule loadModule(VkDevice device, std::string path);

private:
    static std::vector<uint32_t> readModuleFromFile(std::string path);
};