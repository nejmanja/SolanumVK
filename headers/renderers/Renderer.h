#pragma once

#include <memory>

#include "VulkanContext.h"
#include "DescriptorSetAllocator.h"
#include "ImageResources.h"
#include "CommandManager.h"
#include "ImageBindingDescription.h"

class Renderer {
public:
    Renderer(const VulkanContext &vulkanContext, const uint32_t numInputs,
             const std::vector<ImageBindingDescription> inputDescriptions, const uint32_t numOutputs,
             const std::vector<ImageBindingDescription> outputDescriptions)
        : Renderer(vulkanContext, numInputs, numOutputs) {
        this->inputDescriptions = inputDescriptions;
        this->outputDescriptions = outputDescriptions;
    }

    Renderer(const VulkanContext &vulkanContext, const uint32_t numInputs, const uint32_t numOutputs)
        : vulkanContext{vulkanContext}, numInputImages{numInputs}, numOutputImages{numOutputs} {
    }

    virtual ~Renderer() = default;

    // Records and submits rendering commands to the GPU.
    virtual void execute(CommandManager &cmd) = 0;

    void setup(std::vector<ImageResource *> inputs, std::vector<ImageResource *> outputs, double deltaTime) {
        setInputImages(inputs);
        setOutputImages(outputs);

        setup(deltaTime);
    };

protected:
    // Used before recording commands. May include setup for descriptor sets and push constants.
    // Sets the final render target to which the renderer should write.
    virtual void setup(double deltaTime) = 0;


    const VulkanContext &vulkanContext;

    // For pipeline-specific descriptors
    std::unique_ptr<DescriptorSetAllocator> rendererDescriptorAllocator;

    // TODO: add range checks
    [[nodiscard]] ImageResource *getInputImage(const uint32_t index) const { return inputImages[index]; }
    [[nodiscard]] ImageResource *getOutputImage(const uint32_t index) const { return outputImages[index]; }

private:
    // TODO: add debug descriptors in case ranges don't match
    std::vector<ImageResource *> inputImages{}, outputImages{};
    std::vector<ImageBindingDescription> inputDescriptions{}, outputDescriptions{};
    const uint32_t numInputImages{}, numOutputImages{};

    // TODO: add range checks
    void setInputImages(std::vector<ImageResource *> &inputs) { inputImages = std::move(inputs); }
    void setOutputImages(std::vector<ImageResource *> &outputs) { outputImages = std::move(outputs); }
};
