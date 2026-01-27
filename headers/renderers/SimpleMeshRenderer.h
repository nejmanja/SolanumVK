#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "IRenderer.h"
#include "VulkanContext.h"
#include "GraphicsPipeline.h"
#include "SimpleMeshData.h"
#include "MeshUploader.h"
#include "ScopedVkMemoryManager.h"

class SimpleMeshRenderer : public IRenderer {
public:
    SimpleMeshRenderer(const VulkanContext &vulkanContext, VkDescriptorSetLayout sceneDescriptorLayout,
                       VkDescriptorSet sceneDescriptorSet);

    ~SimpleMeshRenderer() override;

    void setup(ImageResource *finalTarget, double deltaTime) override;

    void execute(VkCommandBuffer cmd) override;

private:
    void createDepthTarget();

    void createDescriptors();

    void transitionDepthTarget(VkCommandBuffer cmd);

    void buildPipeline(VkDescriptorSetLayout sceneDescriptorLayout);

    const VkDescriptorSet sceneDescriptorSet;

    VkViewport viewport;
    VkRect2D scissor;

    SimpleMeshData meshData;

    AllocatedImageResource *depthTarget;
    VkRenderingAttachmentInfo depthAttachmentInfo;

    // Pipeline with which rendering will happen
    std::unique_ptr<GraphicsPipeline> pipeline;

    ScopedVkMemoryManager memoryManager;

    struct Transform {
        glm::mat4 model;
    };

    Transform transform;
    AllocatedBuffer transformBuffer;
    VkDescriptorSetLayout transformUniformLayout{VK_NULL_HANDLE};
    VkDescriptorSet transformUniformDescriptorSet{VK_NULL_HANDLE};
};
