#include "ShaderLoader.h"

VkShaderModule ShaderLoader::loadModule(VkDevice device, std::string path)
{
    auto shaderCode = readModuleFromFile(path);

    VkShaderModuleCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, .pNext = nullptr};
    // size in bytes!
    createInfo.codeSize = shaderCode.size() * sizeof(uint32_t);
    createInfo.pCode = shaderCode.data();

    VkShaderModule shaderModule{};
    vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);

    return shaderModule;
}

std::vector<uint32_t> ShaderLoader::readModuleFromFile(std::string path)
{
    std::vector<uint32_t> buffer;
    // open the file. With cursor at the end
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        auto msg = std::string("Failed to read shader from file with path") + path;
        throw std::exception(msg.c_str());
    }

    // find what the size of the file is by looking up the location of the cursor
    // because the cursor is at the end, it gives the size directly in bytes
    size_t fileSize = (size_t)file.tellg();

    // spirv expects the buffer to be on uint32, so make sure to reserve a int
    // vector big enough for the entire file
    buffer = std::vector<uint32_t>(fileSize / sizeof(uint32_t));

    // put file cursor at beginning
    file.seekg(0);

    // load the entire file into the buffer
    file.read((char *)buffer.data(), fileSize);

    // now that the file is loaded into the buffer, we can close it
    file.close();

    return buffer;
}
