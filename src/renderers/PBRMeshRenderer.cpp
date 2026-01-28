#include "renderers/PBRMeshRenderer.h"

#include <iostream>

#include "GraphicsPipelineBuilder.h"
#include "MeshLoader.h"

PBRMeshRenderer::PBRMeshRenderer(const VulkanContext &vulkanContext) : Renderer(vulkanContext) {
    buildPipeline();
    meshData = MeshLoader::loadUVMappedMesh("../../assets/textureMappedTorusKnot.glb");

    std::cout << "UV Mesh data count: " + std::to_string(meshData.getVertexCount()) << std::endl;
}

void PBRMeshRenderer::execute(CommandManager &cmd) {
}

void PBRMeshRenderer::buildPipeline() {
    GraphicsPipelineBuilder pipelineBuilder(vulkanContext);
}
