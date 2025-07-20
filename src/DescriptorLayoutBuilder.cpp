#include "DescriptorLayoutBuilder.h"

void DescriptorLayoutBuilder::addBinding(uint32_t bindingIdx, VkDescriptorType descriptorType, uint32_t descriptorCount, VkShaderStageFlags stageFlags)
{
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = bindingIdx;
    binding.descriptorType = descriptorType;
    binding.descriptorCount = 1;
    binding.stageFlags = stageFlags;

    bindings.push_back(binding);
}

VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device, VkShaderStageFlags defaultStageFlags, VkDescriptorSetLayoutCreateFlags createFlags)
{
    for (auto &&binding : bindings)
    {
        if (binding.stageFlags == 0)
            binding.stageFlags = defaultStageFlags;
    }

    VkDescriptorSetLayoutCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, .pNext = nullptr};
    createInfo.flags = createFlags;
    createInfo.bindingCount = (uint32_t)bindings.size();
    createInfo.pBindings = bindings.data();

    VkDescriptorSetLayout layout;

    vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &layout);

    return layout;
}
