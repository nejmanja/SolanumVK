#include "ComputePipeline.h"

ComputePipeline::ComputePipeline(VkDevice device)
    : IPipeline(device)
{
    VkPipelineLayoutCreateInfo layoutCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, .pNext = nullptr};
    layoutCreateInfo.flags = 0; // all flags are extension-specific, nothing here
    // push constants
    layoutCreateInfo.pushConstantRangeCount = 0;
    layoutCreateInfo.pPushConstantRanges = nullptr;
    // descriptor sets
    layoutCreateInfo.setLayoutCount = 0;
    layoutCreateInfo.pSetLayouts = nullptr;

    vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, &layout);

    VkPipelineShaderStageCreateInfo shaderCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .pNext = nullptr};
    shaderCreateInfo.pSpecializationInfo = nullptr;
    shaderCreateInfo.flags = 0;
    shaderCreateInfo.module = nullptr; // TODO shader
    shaderCreateInfo.pName = "main";
    shaderCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;

    VkComputePipelineCreateInfo pipelineCreateInfo{.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, .pNext = nullptr};
    // For derived pipelines, unused here.
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;
    pipelineCreateInfo.layout = layout;
    pipelineCreateInfo.stage = shaderCreateInfo;

    vkCreateComputePipelines(device, nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline);
}

ComputePipeline::~ComputePipeline()
{
    vkDestroyPipelineLayout(device, layout, nullptr);
    vkDestroyPipeline(device, pipeline, nullptr);
}

void ComputePipeline::execute(VkCommandBuffer cmd)
{
    bind(cmd);
}

void ComputePipeline::bind(VkCommandBuffer cmd)
{
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}
