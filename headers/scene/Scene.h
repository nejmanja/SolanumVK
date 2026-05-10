#pragma once
#include <memory>

#include "RenderList.h"
#include "SceneNodeDescriptor.h"
#include "SceneNode.h"

class Scene {
public:
    Scene() = default;

    SceneNodeHandle addNode(const SceneNodeDescriptor &descriptor) {
        rootObjects.emplace_back(std::make_unique<SceneNode>(descriptor));
        renderListStale = descriptor.gpuMesh.has_value();

        return SceneNodeHandle{rootObjects.back().get()};
    }

    const RenderList &getRenderList() {
        if (renderListStale) rebuildRenderList();

        return renderList;
    }

    void logScene() const;

private:
    static void logRootObjectTree(SceneNode *root);

    void rebuildRenderList();

    bool renderListStale{false};
    RenderList renderList{};
    std::deque<std::unique_ptr<SceneNode> > rootObjects{};
};
