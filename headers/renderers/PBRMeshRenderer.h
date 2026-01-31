#pragma once
#include "GraphicsPipeline.h"
#include "Renderer.h"
#include "UVMappedMeshData.h"

class PBRMeshRenderer : public Renderer {
public:
    explicit PBRMeshRenderer(const VulkanContext &vulkanContext);

    void prepareFrame(double deltaTime) override {
    }

protected:
    void initialize() override {
    }

    void setupResources(const CommandManager &cmd) override {
    }

    void draw(const CommandManager &cmd) override {
    }

private:
    void buildPipeline();

    UVMappedMeshData meshData;

    std::unique_ptr<GraphicsPipeline> pipeline;
};
