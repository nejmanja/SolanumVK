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

class SimpleMeshRenderer : public IRenderer
{
public:
    SimpleMeshRenderer(const VulkanContext &vulkanContext);
    ~SimpleMeshRenderer();

    void setup(ImageResource finalTarget, double deltaTime) override;
    void execute(VkCommandBuffer cmd) override;

private:
    void createDescriptors(const VulkanContext &vulkanContext);
    void buildPipeline(const VulkanContext &vulkanContext);

    VkViewport viewport;
    VkRect2D scissor;

    VmaAllocator vmaAllocator;

    SimpleMeshData meshData;

    MeshUploader meshUploader;

    // Pipeline with which rendering will happen
    std::unique_ptr<GraphicsPipeline> pipeline;

    ScopedVkMemoryManager memoryManager;
    std::unique_ptr<BufferAllocator> bufferAllocator;

    struct Transform
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
    Transform transform;
    AllocatedBuffer transformBuffer;
    VkDescriptorSetLayout transformUniformLayout{VK_NULL_HANDLE};
    VkDescriptorSet transformUniformDescriptorSet{VK_NULL_HANDLE};
};