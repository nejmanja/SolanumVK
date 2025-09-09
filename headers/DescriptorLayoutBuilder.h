#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class DescriptorLayoutBuilder {
public:
    DescriptorLayoutBuilder() = default;

    ~DescriptorLayoutBuilder() = default;

    void addBinding(
        uint32_t bindingIdx,
        VkDescriptorType descriptorType,
        uint32_t descriptorCount = 1,
        VkShaderStageFlags stageFlags = 0
    );

    // Default stage flags will be applied to all bindings which didn't have their flags specified upon adding the binding.
    VkDescriptorSetLayout build(
        VkDevice device,
        VkShaderStageFlags defaultStageFlags,
        VkDescriptorSetLayoutCreateFlags createFlags
    );

    void clear() { bindings.clear(); }

private:
    std::vector<VkDescriptorSetLayoutBinding> bindings;
};
