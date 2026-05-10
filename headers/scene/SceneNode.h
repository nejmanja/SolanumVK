#pragma once

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "GPUMesh.h"
#include "SceneNodeDescriptor.h"
#include "SceneNodeHandle.h"
#include "Transform.h"

class SceneNode {
public:
    // Creates an empty node with an origin transform
    SceneNode() = default;

    SceneNode(SceneNodeDescriptor descriptor)
        : mesh(std::move(descriptor.gpuMesh)), name(std::move(descriptor.name)),
          localTransform(std::move(descriptor.transform)), globalTransform(std::move(localTransform)) {
    }


    SceneNode(SceneNodeDescriptor descriptor, SceneNode *parent) : mesh(std::move(descriptor.gpuMesh)),
                                                                   name(std::move(descriptor.name)), parent(parent),
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

    void setMeshData(GPUMesh meshData) {
        mesh = meshData;
    }

    [[nodiscard]] const std::deque<std::unique_ptr<SceneNode> > &getChildren() const {
        return children;
    }

    [[nodiscard]] const std::string &getName() const { return name; }

    [[nodiscard]] std::string toString();

    const Transform &getLocalTransform() const { return localTransform; }

    Transform &getGlobalTransform() {
        if (globalTransformStale)
            recalculateGlobalTransform();

        return globalTransform;
    }

    glm::vec3 getGlobalPosition() {
        return getGlobalTransform().getTranslation();
    }

    glm::vec3 getLocalPosition() const { return localTransform.getTranslation(); }

    void setLocalPosition(glm::vec3 localPosition);

    [[nodiscard]] const std::optional<GPUMesh> &getMeshData() const { return mesh; }

    [[nodiscard]] glm::mat4 getLocalTransformMatrix() { return localTransform.getTransformMatrix(); }

    void setLocalTransformMatrix(glm::mat4 mat);

    [[nodiscard]] glm::mat4 getGlobalTransformMatrix() { return getGlobalTransform().getTransformMatrix(); }

private:
    // Mark entire subtree of transforms as stale. This may happen if a node's local transform changes,
    // which invalidates all global transforms of the children
    void markChildrenGlobalTransformsStale() const;

    void recalculateGlobalTransform();

    std::optional<GPUMesh> mesh{};

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
