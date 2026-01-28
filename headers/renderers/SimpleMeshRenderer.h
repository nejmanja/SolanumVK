#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "VulkanContext.h"
#include "GraphicsPipeline.h"
#include "SimpleMeshData.h"
#include "MeshUploader.h"
#include "ScopedVkMemoryManager.h"

class SimpleMeshRenderer : public Renderer {
public:
    SimpleMeshRenderer(const VulkanContext &vulkanContext, VkDescriptorSetLayout sceneDescriptorLayout,
                       VkDescriptorSet sceneDescriptorSet);

    ~SimpleMeshRenderer() override;


    void execute(CommandManager &cmd) override;

private:
    static constexpr uint32_t TARGET_IMAGE_INDEX = 0;
    [[nodiscard]] ImageResource *getOutput() const { return getOutputImage(TARGET_IMAGE_INDEX); }

    void setup(double deltaTime) override;

    void createDepthTarget();

    void createDescriptors();

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
