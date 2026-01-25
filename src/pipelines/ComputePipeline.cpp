#include "ComputePipeline.h"

#include <glm/vec3.hpp>

#include "ShaderLoader.h"
#include "VulkanUtils.h"

ComputePipeline::ComputePipeline(VkDevice device, VkDescriptorSetLayout descriptorSetLayout)
    : IPipeline(device) {
    VkPipelineLayoutCreateInfo layoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, .pNext = nullptr
    };
    layoutCreateInfo.flags = 0; // all flags are extension-specific, nothing here
    // push constants
    VkPushConstantRange pushConstant;
    pushConstant.offset = 0;
    pushConstant.size = sizeof(glm::vec3);
    pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    layoutCreateInfo.pushConstantRangeCount = 1;
    layoutCreateInfo.pPushConstantRanges = &pushConstant;
    // descriptor sets
    layoutCreateInfo.setLayoutCount = 1;
    layoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    auto result = vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, &layout);
    VulkanUtils::CheckVkResult(result);

    auto shaderModule = ShaderLoader::loadModule(device, "../../shaders/gradient.comp.spv");
    VkPipelineShaderStageCreateInfo shaderCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .pNext = nullptr
    };
    shaderCreateInfo.pSpecializationInfo = nullptr;
    shaderCreateInfo.flags = 0;
    shaderCreateInfo.module = shaderModule;
    shaderCreateInfo.pName = "main";
    shaderCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;

    VkComputePipelineCreateInfo pipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, .pNext = nullptr
    };
    // For derived pipelines, unused here.
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;
    pipelineCreateInfo.layout = layout;
    pipelineCreateInfo.stage = shaderCreateInfo;

    result = vkCreateComputePipelines(device, nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline);
    VulkanUtils::CheckVkResult(result);
    vkDestroyShaderModule(device, shaderModule, nullptr);
}

ComputePipeline::~ComputePipeline() {
    vkDestroyPipelineLayout(device, layout, nullptr);
    vkDestroyPipeline(device, pipeline, nullptr);
}

void ComputePipeline::execute() {
    // TODO: pass some info about the target of the pipeline as pipeline state when binding
    vkCmdDispatch(boundCommandBuffer, std::ceil(800 / 16.0), std::ceil(600 / 16.0), 1);
}

void ComputePipeline::bindPushConstants(const void *pushConstantData, size_t size) {
    IPipeline::bindPushConstants(pushConstantData, size, VK_SHADER_STAGE_COMPUTE_BIT);
}

void ComputePipeline::bindDescriptorSets(uint32_t setCount, VkDescriptorSet *sets) {
    IPipeline::bindDescriptorSets(setCount, sets, VK_PIPELINE_BIND_POINT_COMPUTE);
}

void ComputePipeline::bind(VkCommandBuffer cmd) {
    IPipeline::bind(cmd, VK_PIPELINE_BIND_POINT_COMPUTE);
}
