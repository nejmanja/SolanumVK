#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "DescriptorMemoryManager.h"

class DescriptorModule {
public:
    DescriptorModule(const VkDevice device, const VkDescriptorSetLayout layout,
                     const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings) : device(device),
        layout(layout),
        layoutBindings(layoutBindings) {
    }

    VkDescriptorSet createSet(DescriptorMemoryManager &memoryManager) {
        descriptorSets.push_back(memoryManager.allocate(layout));
        return descriptorSets.back();
    }

    [[nodiscard]] VkDescriptorSet getDescriptorSet(const uint32_t index) const { return descriptorSets[index]; }

    [[nodiscard]] const VkDescriptorSet *getDescriptorSetPtr(const uint32_t index) const {
        return &descriptorSets[index];
    }

    [[nodiscard]] VkDescriptorSetLayout getLayout() const { return layout; }

    [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding> &getBindings() const { return layoutBindings; }

    ~DescriptorModule();

private:
    const VkDevice device;
    VkDescriptorSetLayout layout{};
    const std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    std::vector<VkDescriptorSet> descriptorSets{};
};
