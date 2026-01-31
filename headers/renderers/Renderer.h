#pragma once

#include <memory>

#include "VulkanContext.h"
#include "DescriptorSetAllocator.h"
#include "ImageResources.h"
#include "CommandManager.h"
#include "ImageBindingDescription.h"

class Renderer {
public:
    Renderer(const VulkanContext &vulkanContext,
             const std::vector<ImageBindingDescription> &inputDescriptions,
             const std::vector<ImageBindingDescription> &outputDescriptions)
        : Renderer(vulkanContext, inputDescriptions.size(), outputDescriptions.size()) {
        this->inputDescriptions = inputDescriptions;
        this->outputDescriptions = outputDescriptions;
    }

    Renderer(const VulkanContext &vulkanContext, const uint32_t numInputs, const uint32_t numOutputs)
        : vulkanContext{vulkanContext}, numInputImages{numInputs}, numOutputImages{numOutputs} {
    }

    virtual ~Renderer() = default;

    // Records and submits rendering commands to the GPU.
    void execute(CommandManager &cmd) {
        setupResources(cmd);
        draw(cmd);
    }

    // Used once at the beginning of lifetime - setting I/O is done here
    // TODO: pass global scene data here as well
    void initialize(std::vector<ImageResource *> inputs, std::vector<ImageResource *> outputs) {
        setInputImages(inputs);
        setOutputImages(outputs);

        initialize();
    };

    // Used for preparing the frame before rendering - per-frame resources should be set up in overrides
    // delta-time can be used for updating any internal time-based state.
    virtual void prepareFrame(double deltaTime) = 0;

protected:
    // Used once at the beginning of lifetime - "bind-once" resources should be set up in overrides
    virtual void initialize() = 0;

    // Setup resources needed for the frame - write & bind the per-renderer state and global scene state
    virtual void setupResources(const CommandManager &cmd) = 0;

    // Write & bind per-draw state (if applicable) and do the actual rendering
    virtual void draw(const CommandManager &cmd) = 0;

    const VulkanContext &vulkanContext;

    // For pipeline-specific descriptors
    std::unique_ptr<DescriptorSetAllocator> rendererDescriptorAllocator;

    // TODO: add range checks
    [[nodiscard]] ImageResource *getInputImage(const uint32_t index) const { return inputImages[index]; }
    [[nodiscard]] ImageResource *getOutputImage(const uint32_t index) const { return outputImages[index]; }

private:
    std::vector<ImageResource *> inputImages{}, outputImages{};
    std::vector<ImageBindingDescription> inputDescriptions{}, outputDescriptions{};
    const uint32_t numInputImages{}, numOutputImages{};

    void setInputImages(std::vector<ImageResource *> &inputs);

    void setOutputImages(std::vector<ImageResource *> &outputs);
};
