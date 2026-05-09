#pragma once
#include <deque>

#include "SceneNodeHandle.h"

class RenderList {
public:
    RenderList() = default;

    void addNode(const SceneNodeHandle handle) { renderNodes.push_back(handle); }

private:
    std::deque<SceneNodeHandle> renderNodes{};
};
