#pragma once

#include "VulkanContext.h"
#include "GraphicsPipeline.h"
#include "VertexAttributes.h"

#include <vector>
#include <memory>

class GraphicsPipelineBuilder {
public:
    explicit GraphicsPipelineBuilder(const VulkanContext &vulkanContext);

    ~GraphicsPipelineBuilder() {
        reset();
    }

    // Descriptor set layouts
    void addDescriptorSetLayout(VkDescriptorSetLayout layout) { descriptorSetLayouts.push_back(layout); }

    void resetDescriptorSetLayouts();

    // Push constants
    void addPushConstantRange(const VkPushConstantRange range) { pushConstantRanges.push_back(range); }
    void resetPushConstantRanges() { pushConstantRanges.clear(); }

    // Create an entire binding with all attributes contained within it
    void addVertexBinding(uint32_t binding, uint32_t stride, std::vector<VkVertexInputAttributeDescription> attributes);

    void addVertexBinding(const VertexBinding &binding);

    void resetVertexBindings();

    // Attachments
    void resetColorAttachments() { colorAttachmentFormats.clear(); }
    void addColorAttachmentFormat(const VkFormat format) { colorAttachmentFormats.push_back(format); }
    void setDepthAttachmentFormat(const VkFormat format) { depthFormat = format; }
    void setStencilAttachmentFormat(const VkFormat format) { stencilFormat = format; }

    // Shader modules
    void addShaderModule(const char *shaderName, const char *entryPointName, VkShaderStageFlagBits stage);

    void resetShaderModules();

    // Rasterization state
    void setCullMode(const VkCullModeFlagBits cullMode) { rasterizationInfo.cullMode = cullMode; }
    void setFrontFace(const VkFrontFace frontFace) { rasterizationInfo.frontFace = frontFace; }

    // Depth/stencil state
    void enableDepthTest(const bool state) { depthStencilInfo.depthTestEnable = state; }
    void enableDepthWrite(const bool state) { depthStencilInfo.depthWriteEnable = state; }

    // Color blend state
    void enableBlending(const bool state) { colorBlendState.blendEnable = state; }

    void setColorBlendFunction(VkBlendFactor src, VkBlendFactor dst, VkBlendOp op);

    void setAlphaBlendFunction(VkBlendFactor src, VkBlendFactor dst, VkBlendOp op);

    // Dynamic state
    void addDynamicState(const VkDynamicState state) { dynamicState.push_back(state); }
    void resetDynamicState() { dynamicState.clear(); }

    std::unique_ptr<GraphicsPipeline> build();

    void reset();

private:
    // =======================================================================================
    // Custom, intermediary data to be fed to the pipeline
    // =======================================================================================
    const VkDevice device;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
    std::vector<VkPushConstantRange> pushConstantRanges{};

    std::vector<VkFormat> colorAttachmentFormats{};
    VkFormat depthFormat{VK_FORMAT_UNDEFINED};
    VkFormat stencilFormat{VK_FORMAT_UNDEFINED};

    // These two hold the vertex format
    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

    // Shader stages, containing various shader modules
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

    std::vector<VkDynamicState> dynamicState{};

    // =======================================================================================
    // Raw, cached vulkan structs
    // =======================================================================================

    VkPipelineLayoutCreateInfo layoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // will most likely always remain 0
        // descriptor sets and push constant ranges will be assigned dynamically, so 0-init is fine.
    };

    // Dynamic rendering info
    VkPipelineRenderingCreateInfo renderingCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr
    };

    // vertex format will get filled into this structure upon building the pipeline
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0 // reserved for future use
    };

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

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = {},
        .maxDepthBounds = {}
    };

    VkPipelineColorBlendAttachmentState colorBlendState{
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1, // TODO: create arbitrary amount of blend states...
        .pAttachments = &colorBlendState,
        .blendConstants = {0, 0, 0, 0}
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // reserved for future use
    };

    // =======================================================================================
    // Hard-coded stuff (at least for now)...
    // =======================================================================================
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    VkViewport initialViewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = 800.0f,
        .height = 600.0f,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D initialScissor{
        .offset = {0, 0},
        .extent = {800, 600}
    };

    VkPipelineViewportStateCreateInfo viewportInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0, // reserved for future use
        .viewportCount = 1,
        .pViewports = &initialViewport,
        .scissorCount = 1,
        .pScissors = &initialScissor
    };

    // No MSAA (for now)...
    VkPipelineMultisampleStateCreateInfo multisampleInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    // =======================================================================================
    // Pipeline create info, will directly result in a pipeline
    // =======================================================================================
    VkGraphicsPipelineCreateInfo pipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderingCreateInfo,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pTessellationState = VK_NULL_HANDLE,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterizationInfo,
        .pMultisampleState = &multisampleInfo,
        .pDepthStencilState = VK_NULL_HANDLE, // will be setup upon building
        .pColorBlendState = &colorBlendInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = VK_NULL_HANDLE,
        // Unused, we're using dynamic rendering
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        // Unused, for derived pipelines
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0
    };
};
