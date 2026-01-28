#pragma once
#include "GraphicsPipeline.h"
#include "Renderer.h"
#include "UVMappedMeshData.h"

class PBRMeshRenderer : public Renderer {
public:
    explicit PBRMeshRenderer(const VulkanContext &vulkanContext);

    void execute(CommandManager &cmd) override;

private:
    void buildPipeline();

    void setup(double deltaTime) override {
    };

    UVMappedMeshData meshData;

    std::unique_ptr<GraphicsPipeline> pipeline;
};
