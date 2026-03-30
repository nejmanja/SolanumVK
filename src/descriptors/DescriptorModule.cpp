#include "DescriptorModule.h"

DescriptorModule::~DescriptorModule() {
    // The sets will be freed implicitly when pool gets destroyed...
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
}
