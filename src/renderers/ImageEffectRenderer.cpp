#include "ImageEffectRenderer.h"

#include "DescriptorLayoutBuilder.h"
#include "DescriptorWriter.h"
#include "GraphicsPipelineBuilder.h"
#include "SolanumConstants.h"
#include "VulkanUtils.h"

void ImageEffectRenderer::createPipeline() {
    auto builder = GraphicsPipelineBuilder{vulkanContext};
    builder.addColorAttachmentFormat(getOutputImage()->getFormat());
    builder.enableDepthWrite(false);
    builder.enableDepthTest(false);
    builder.enableBlending(false);
    builder.addShaderModule("../../shaders/imageEffect.vert.spv", "main", VK_SHADER_STAGE_VERTEX_BIT);
    builder.addShaderModule("../../shaders/invertImageEffect.frag.spv", "main", VK_SHADER_STAGE_FRAGMENT_BIT);
    builder.setCullMode(VK_CULL_MODE_BACK_BIT);
    builder.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    builder.addDynamicState(VK_DYNAMIC_STATE_SCISSOR);
    builder.addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);

    builder.addDescriptorSetLayout(descriptorLayout);

    pipeline = builder.build();
}

void ImageEffectRenderer::createDescriptors() {
    DescriptorLayoutBuilder layoutBuilder{};
    layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    descriptorLayout = layoutBuilder.build(vulkanContext.getDevice(), VK_SHADER_STAGE_FRAGMENT_BIT, 0);
    memoryManager.registerResource(descriptorLayout);

    auto resourceSizes = std::vector<DescriptorSetAllocator::PoolResourceSizePerSet>{
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}
    };
    rendererDescriptorAllocator = std::make_unique<DescriptorSetAllocator>(vulkanContext.getDevice(), resourceSizes);

    descriptorSet = rendererDescriptorAllocator->allocate(descriptorLayout);

    DescriptorWriter::writeImage(vulkanContext, descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                 getInputImage()->getImageView(), inputFormat, inputImageSampler);
}


void ImageEffectRenderer::createInputSampler() {
    VkSamplerCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        // no need for trilinear as this thing will have no mips
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        // doesn't matter, 2d texture
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0,
        // no need for aniso
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 0,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0,
        .maxLod = 0,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };
    VulkanUtils::CheckVkResult(vkCreateSampler(vulkanContext.getDevice(), &info, VK_NULL_HANDLE, &inputImageSampler));
}

void ImageEffectRenderer::initialize() {
    createInputSampler();
    createDescriptors();
    createPipeline();

    const auto *output = getOutputImage();

    colorAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = output->getImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        // no MSAA
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD, // compute pass will draw before this :)
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {}
    };

    renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = VkRect2D{
            .offset = {0, 0}, .extent = {output->getExtent().width, output->getExtent().height}
        },
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = VK_NULL_HANDLE,
        .pStencilAttachment = VK_NULL_HANDLE
    };
}

void ImageEffectRenderer::setupResources(const CommandManager &cmd) {
    getInputImage()->transition(cmd, inputFormat);
    getOutputImage()->transition(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void ImageEffectRenderer::draw(const CommandManager &cmd) {
    const auto cmdBuffer = cmd.get();
    const auto *output = getOutputImage();

    colorAttachmentInfo.imageView = output->getImageView();
    renderingInfo.renderArea = VkRect2D{
        .offset = {0, 0}, .extent = {output->getExtent().width, output->getExtent().height}
    };

    vkCmdBeginRendering(cmdBuffer, &renderingInfo);
    // bind pipeline
    pipeline->bind(cmdBuffer);
    // bind descriptors
    pipeline->bindDescriptorSets(1, &descriptorSet);
    // render
    // TODO: this execute method is dumb (it does literally nothing), remove this entirely
    pipeline->execute();
    // Render 3 vertices, the triangle is hard-coded in the shader :D
    vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

    vkCmdEndRendering(cmdBuffer);
}
