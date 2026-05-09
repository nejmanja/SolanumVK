#pragma once

#include <deque>
#include <memory>
#include <string>
#include <utility>

#include "SceneNodeDescriptor.h"
#include "SceneNodeHandle.h"
#include "Transform.h"

class SceneNode {
public:
    // Creates an empty node with an origin transform
    SceneNode() = default;

    explicit SceneNode(SceneNodeDescriptor descriptor)
        : name(std::move(descriptor.name)), localTransform(std::move(descriptor.transform)),
          globalTransform(std::move(localTransform)) {
    }

    SceneNode(SceneNodeDescriptor descriptor, SceneNode *parent) : name(std::move(descriptor.name)), parent(parent),
                                                                   localTransform(descriptor.transform),
                                                                   globalTransform(parent->globalTransform.
                                                                       getTransformMatrix() * descriptor.transform.
                                                                       getTransformMatrix()) {
    }

    // No copying for now...
    SceneNode(SceneNode &) = delete;

    SceneNodeHandle addChild(const SceneNodeDescriptor &descriptor) {
        children.emplace_back(std::make_unique<SceneNode>(descriptor, this));
        return SceneNodeHandle{children.back().get()};
    }


    [[nodiscard]] const std::deque<std::unique_ptr<SceneNode> > &getChildren() const {
        return children;
    }

    [[nodiscard]] const std::string &getName() const { return name; }

private:
    std::string name{};
    // Deque to avoid pointer reallocs on resize, which would happen with a std::vector
    std::deque<std::unique_ptr<SceneNode> > children{};
    SceneNode *parent{};
    // Transform local to this node
    Transform localTransform{};
    // Cached global transform, updated lazily on getter
    Transform globalTransform{};
    // When a parent transform changes, all of its children become stale
    // The cached global transform should get updated on first call
    bool globalTransformStale{false};
};
