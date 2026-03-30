#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "DescriptorModule.h"

class DescriptorLayoutBindings {
public:
    DescriptorLayoutBindings() = default;

    ~DescriptorLayoutBindings() = default;

    void addBinding(
        uint32_t bindingIdx,
        VkDescriptorType descriptorType,
        uint32_t descriptorCount = 1,
        VkShaderStageFlags stageFlags = 0
    );

    // Default stage flags will be applied to all bindings which didn't have their flags specified upon adding the binding.
    std::unique_ptr<DescriptorModule> createModule(
        VkDevice device,
        VkShaderStageFlags defaultStageFlags,
        VkDescriptorSetLayoutCreateFlags createFlags = 0
    );

    void clear() { bindings.clear(); }

private:
    std::vector<VkDescriptorSetLayoutBinding> bindings;
};
