#pragma once
#include <deque>

#include "SceneNodeHandle.h"

class RenderList {
public:
    RenderList() = default;

    void addNode(const SceneNodeHandle handle) { renderNodes.push_back(handle); }

    void clear() { renderNodes.clear(); }

    auto begin() { return renderNodes.begin(); }
    auto end() { return renderNodes.end(); }
    auto begin() const { return renderNodes.begin(); }
    auto end() const { return renderNodes.end(); }

private:
    // These are just handles, the render list DOES NOT own memory of the actual nodes!
    std::deque<SceneNodeHandle> renderNodes{};
};
