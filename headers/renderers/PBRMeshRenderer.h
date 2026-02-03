#pragma once
#include "GraphicsPipeline.h"
#include "MeshData.h"
#include "Renderer.h"

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

    std::unique_ptr<MeshData> meshData;

    std::unique_ptr<GraphicsPipeline> pipeline;
};
