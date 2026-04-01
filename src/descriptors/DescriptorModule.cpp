#include "DescriptorModule.h"

DescriptorModule::~DescriptorModule() {
    // The sets will be freed implicitly when pool gets destroyed...
    if (layout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(device, layout, nullptr);
}
