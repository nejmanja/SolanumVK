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
        renderListStale = true;

        return SceneNodeHandle{rootObjects.back().get()};
    }

    void logScene();

    static void logRootObjectTree(SceneNode *root);

private:
    bool renderListStale{false};
    RenderList renderList{};
    std::deque<std::unique_ptr<SceneNode> > rootObjects{};
};
