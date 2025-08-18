#include "GraphicsPipeline.h"

#include "VulkanUtils.h"
#include "ShaderLoader.h"

GraphicsPipeline::GraphicsPipeline(VkDevice device, VkDescriptorSetLayout descriptorSetLayout)
    : IPipeline(device)
{
    VkPipelineLayoutCreateInfo layoutCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, .pNext = nullptr};
    layoutCreateInfo.flags = 0; // all flags are extension-specific, nothing here
    // push constants
    layoutCreateInfo.pushConstantRangeCount = 0;
    layoutCreateInfo.pPushConstantRanges = nullptr;
    // descriptor sets
    layoutCreateInfo.setLayoutCount = 1;
    layoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    auto result = vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, &layout);
    VulkanUtils::CheckVkResult(result);

    // TODO: Shaders...
    auto vertexShaderModule = ShaderLoader::loadModule(device, "../shaders/gradient.comp.spv");
    VkPipelineShaderStageCreateInfo vertexShaderInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .pNext = nullptr};
    vertexShaderInfo.pSpecializationInfo = nullptr;
    vertexShaderInfo.flags = 0;
    vertexShaderInfo.module = vertexShaderModule;
    vertexShaderInfo.pName = "main";
    vertexShaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo fragmentShaderInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .pNext = nullptr};

    VkPipelineShaderStageCreateInfo stages[2] = {vertexShaderInfo, fragmentShaderInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, .pNext = nullptr};
    vertexInputInfo.flags = 0; // reserved for future use
    // Vertices will be hard-coded in the shader, these will be null
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = VK_NULL_HANDLE;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = VK_NULL_HANDLE;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, .pNext = nullptr};
    inputAssemblyInfo.flags = 0; // reserved for future use
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = 800.0f, // TODO: propagate viewport size to here (or just use dynamic viewport teehee)
        .height = 600.0f,
        .minDepth = 0.0f,
        .maxDepth = 1.0f};

    VkRect2D scissor{
        .offset = {0, 0},
        .extent = {800, 600} // TODO: propagate viewport size to here (or just use dynamic viewport teehee)
    };

    VkPipelineViewportStateCreateInfo viewportInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, .pNext = nullptr};
    viewportInfo.flags = 0; // reserved for future use
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // reserved for future use
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        // Unused
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,

        .lineWidth = 1.0f,
    };

    // Everything off, no multisampling
    VkPipelineMultisampleStateCreateInfo multisampleInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, .pNext = nullptr};
    multisampleInfo.sampleShadingEnable = VK_FALSE;
    multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleInfo.minSampleShading = 1.0f;
    multisampleInfo.pSampleMask = nullptr;
    multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleInfo.alphaToOneEnable = VK_FALSE;

    // Default one-zero blending
    VkPipelineColorBlendAttachmentState colorBlendState{};
    colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendState.blendEnable = VK_FALSE;
    colorBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.logicOpEnable = VK_FALSE;
    colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendState;
    colorBlendInfo.blendConstants[0] = 0.0f;
    colorBlendInfo.blendConstants[1] = 0.0f;
    colorBlendInfo.blendConstants[2] = 0.0f;
    colorBlendInfo.blendConstants[3] = 0.0f;

    // TODO: pass attachment format here as well
    VkFormat colorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    VkPipelineRenderingCreateInfo renderingCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO, .pNext = nullptr};
    renderingCreateInfo.colorAttachmentCount = 1;
    renderingCreateInfo.pColorAttachmentFormats = &colorFormat;
    renderingCreateInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;   // no depth
    renderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED; // no stencil
    renderingCreateInfo.viewMask = 0;                                  // no multiview chicanery

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderingCreateInfo, // for dynamic rendering
        .flags = 0,
        .stageCount = 2, // vertex, framgent
        .pStages = stages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pTessellationState = VK_NULL_HANDLE, // no tessellation
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterizationInfo,
        .pMultisampleState = &multisampleInfo,
        .pDepthStencilState = VK_NULL_HANDLE, // no depth/stencil for now
        .pColorBlendState = &colorBlendInfo,
        .pDynamicState = VK_NULL_HANDLE, // TODO: put viewport/scissor here
        .layout = layout,
        // Unused, we're using dynamic rendering
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        // Unused, for derived pipelines
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0};

    result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
    VulkanUtils::CheckVkResult(result);

    vkDestroyShaderModule(device, vertexShaderModule, nullptr);
}

GraphicsPipeline::~GraphicsPipeline()
{
}

void GraphicsPipeline::bindPushConstants(void *pushConstantData)
{
}

void GraphicsPipeline::bindDescriptorSets(uint32_t setCount, VkDescriptorSet *sets)
{
}

void GraphicsPipeline::bind(VkCommandBuffer cmd)
{
}

void GraphicsPipeline::execute()
{
}
