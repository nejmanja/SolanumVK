#include "GraphicsPipeline.h"

#include "VulkanUtils.h"
#include "ShaderLoader.h"

GraphicsPipeline::~GraphicsPipeline() {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, layout, nullptr);
}

void GraphicsPipeline::bindPushConstants(const void *pushConstantData, size_t size) {
    IPipeline::bindPushConstants(pushConstantData, size, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
}

void GraphicsPipeline::bindDescriptorSets(uint32_t setCount, VkDescriptorSet *sets) {
    IPipeline::bindDescriptorSets(setCount, sets, VK_PIPELINE_BIND_POINT_GRAPHICS);
}

void GraphicsPipeline::setViewport(VkViewport *viewport) {
    vkCmdSetViewport(boundCommandBuffer, 0, 1, viewport);
}

void GraphicsPipeline::setScissor(VkRect2D *scissor) {
    vkCmdSetScissor(boundCommandBuffer, 0, 1, scissor);
}

void GraphicsPipeline::bind(VkCommandBuffer cmd) {
    IPipeline::bind(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS);
}

void GraphicsPipeline::execute() {
}
