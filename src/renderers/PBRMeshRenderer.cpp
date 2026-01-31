#include "renderers/PBRMeshRenderer.h"

#include <iostream>

#include "GraphicsPipelineBuilder.h"
#include "MeshLoader.h"

PBRMeshRenderer::PBRMeshRenderer(const VulkanContext &vulkanContext)
    : Renderer(vulkanContext, 0, 1) {
    buildPipeline();
    meshData = MeshLoader::loadUVMappedMesh("../../assets/textureMappedTorusKnot.glb");

    std::cout << "UV Mesh data count: " + std::to_string(meshData.getVertexCount()) << std::endl;
}


void PBRMeshRenderer::buildPipeline() {
    GraphicsPipelineBuilder pipelineBuilder(vulkanContext);
}
