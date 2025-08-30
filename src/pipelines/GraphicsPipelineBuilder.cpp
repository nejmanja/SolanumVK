#include "GraphicsPipelineBuilder.h"

#include "ShaderLoader.h"
#include "VulkanUtils.h"

GraphicsPipelineBuilder::GraphicsPipelineBuilder(const VulkanContext &vulkanContext)
    : device(vulkanContext.getDevice())
{
}

void GraphicsPipelineBuilder::resetDescriptorSetLayouts()
{
    for (auto &&descriptorSetLayout : descriptorSetLayouts)
    {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    }
    descriptorSetLayouts.clear();
}

void GraphicsPipelineBuilder::addVertexBinding(uint32_t binding, uint32_t stride, std::vector<VkVertexInputAttributeDescription> attributes)
{
    VkVertexInputBindingDescription bindingDescription{
        .binding = binding,
        .stride = stride,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

    bindingDescriptions.push_back(bindingDescription);

    for (auto &&attribute : attributes)
    {
        VkVertexInputAttributeDescription attributeDescription{
            .location = attribute.location,
            .binding = binding,
            .format = attribute.format,
            .offset = attribute.offset};

        attributeDescriptions.push_back(attributeDescription);
    }
}

void GraphicsPipelineBuilder::addVertexBinding(const VertexBinding &binding)
{
    bindingDescriptions.push_back(binding.createBindingDescription());

    for (auto &&attribute : binding.getAttributes())
    {
        attributeDescriptions.push_back(attribute.attributeDescription);
    }
}

inline void GraphicsPipelineBuilder::resetVertexBindings()
{
    bindingDescriptions.clear();
    attributeDescriptions.clear();
}

void GraphicsPipelineBuilder::addShaderModule(const char *shaderPath, const char *entryPointName, VkShaderStageFlagBits stage)
{
    auto shaderModule = ShaderLoader::loadModule(device, shaderPath);
    VkPipelineShaderStageCreateInfo stageInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = stage,
        .module = shaderModule,
        .pName = entryPointName,
    };

    shaderStages.push_back(stageInfo);
}

void GraphicsPipelineBuilder::reset()
{
    resetDescriptorSetLayouts();
    resetPushConstantRanges();

    resetColorAttachments();
    resetShaderModules();
    depthFormat = VK_FORMAT_UNDEFINED;
    stencilFormat = VK_FORMAT_UNDEFINED;
}

void GraphicsPipelineBuilder::resetShaderModules()
{
    for (auto &&shaderStage : shaderStages)
    {
        vkDestroyShaderModule(device, shaderStage.module, nullptr);
    }

    shaderStages.clear();
}

void GraphicsPipelineBuilder::setColorBlendFunction(VkBlendFactor src, VkBlendFactor dst, VkBlendOp op)
{
    colorBlendState.srcColorBlendFactor = src;
    colorBlendState.dstColorBlendFactor = dst;
    colorBlendState.colorBlendOp = op;
}

void GraphicsPipelineBuilder::setAlphaBlendFunction(VkBlendFactor src, VkBlendFactor dst, VkBlendOp op)
{
    colorBlendState.srcAlphaBlendFactor = src;
    colorBlendState.dstAlphaBlendFactor = dst;
    colorBlendState.alphaBlendOp = op;
}

std::unique_ptr<GraphicsPipeline> GraphicsPipelineBuilder::build()
{
    // Assign intermediary state to the layout
    layoutCreateInfo.setLayoutCount = descriptorSetLayouts.size();
    layoutCreateInfo.pSetLayouts = layoutCreateInfo.setLayoutCount > 0 ? descriptorSetLayouts.data() : VK_NULL_HANDLE;
    layoutCreateInfo.pushConstantRangeCount = pushConstantRanges.size();
    layoutCreateInfo.pPushConstantRanges = layoutCreateInfo.pushConstantRangeCount > 0 ? pushConstantRanges.data() : VK_NULL_HANDLE;

    // Build the layout...
    VkPipelineLayout pipelineLayout{};
    vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, &pipelineLayout);
    // ...and actually assign it!
    pipelineCreateInfo.layout = pipelineLayout;

    // Assign intermediary state to pipeline accordingly
    vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();

    // only assign depthStencilState if we actually have depth or stencil
    pipelineCreateInfo.pDepthStencilState = depthFormat != VK_FORMAT_UNDEFINED || stencilFormat != VK_FORMAT_UNDEFINED
                                                ? &depthStencilInfo
                                                : VK_NULL_HANDLE;

    dynamicStateInfo.dynamicStateCount = dynamicState.size();
    dynamicStateInfo.pDynamicStates = dynamicStateInfo.dynamicStateCount > 0 ? dynamicState.data() : VK_NULL_HANDLE;

    VkPipeline pipeline{};
    auto result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
    VulkanUtils::CheckVkResult(result);

    return std::make_unique<GraphicsPipeline>(device, pipelineLayout, pipeline);
}
