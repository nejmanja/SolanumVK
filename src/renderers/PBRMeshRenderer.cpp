#include "renderers/PBRMeshRenderer.h"

#include <iostream>

#include "GraphicsPipelineBuilder.h"
#include "MeshLoader.h"

PBRMeshRenderer::PBRMeshRenderer(const VulkanContext &vulkanContext)
    : Renderer(vulkanContext, 0, 1),
      meshData(MeshLoader::loadMesh(
          VertexAttributes::Position | VertexAttributes::Normal | VertexAttributes::UV0,
          "../../assets/textureMappedTorusKnot.glb")) {
    buildPipeline();

    std::cout << "UV Mesh data count: " + std::to_string(meshData->getVertexCount()) << std::endl;
}


void PBRMeshRenderer::buildPipeline() {
    GraphicsPipelineBuilder pipelineBuilder(vulkanContext);
}
