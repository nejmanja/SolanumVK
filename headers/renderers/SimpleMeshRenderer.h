#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DescriptorModule.h"
#include "VulkanContext.h"
#include "GraphicsPipeline.h"
#include "Scene.h"
#include "ScopedVkMemoryManager.h"
#include "SimpleRenderer.h"

class SimpleMeshRenderer : public SimpleRenderer {
public:
    SimpleMeshRenderer(const VulkanContext &vulkanContext, VkDescriptorSetLayout sceneDescriptorLayout,
                       VkDescriptorSet sceneDescriptorSet);

    ~SimpleMeshRenderer() override;

private:
    void initialize() override;

    void createDepthTarget();

    void createDescriptors();

    void buildPipeline(VkDescriptorSetLayout sceneDescriptorLayout);

protected:
    void setupResources(const CommandManager &cmd) override;

    void prepareFrame(double deltaTime) override;

    void draw(const CommandManager &cmd) override;

private:
    const VkDescriptorSet sceneDescriptorSet;

    VkViewport viewport;
    VkRect2D scissor;

    std::optional<MeshData> meshData;

    AllocatedImageResource *depthTarget;
    VkRenderingAttachmentInfo depthAttachmentInfo;

    // Pipeline with which rendering will happen
    std::unique_ptr<GraphicsPipeline> pipeline;

    ScopedVkMemoryManager memoryManager;

    AllocatedBuffer transformBuffer, otherTransformBuffer;
    std::optional<DescriptorModule> descriptorModule;

    Scene scene{};
    SceneNodeHandle monkeyNode{};
};
