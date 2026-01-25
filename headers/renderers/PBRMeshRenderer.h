#pragma once
#include "GraphicsPipeline.h"
#include "IRenderer.h"
#include "UVMappedMeshData.h"

class PBRMeshRenderer : public IRenderer {
public:
    explicit PBRMeshRenderer(const VulkanContext &vulkanContext);

    void execute(VkCommandBuffer cmd) override;

private:
    void buildPipeline();

    UVMappedMeshData meshData;

    std::unique_ptr<GraphicsPipeline> pipeline;
};
