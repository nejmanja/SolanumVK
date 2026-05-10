#pragma once

// Fwd declaration to prevent circular dependency w/ SceneNode
class SceneNode;

class SceneNodeHandle {
public:
    SceneNodeHandle() : node(nullptr) {
    }

    explicit SceneNodeHandle(SceneNode *node) : node(node) {
    }

    SceneNodeHandle(const SceneNodeHandle &other)
        : node(other.node) {
    }

    SceneNodeHandle(SceneNodeHandle &&other) noexcept
        : node(other.node) {
        other.node = nullptr;
    }

    SceneNodeHandle &operator=(const SceneNodeHandle &other) {
        if (this == &other)
            return *this;
        node = other.node;
        return *this;
    }

    SceneNodeHandle &operator=(SceneNodeHandle &&other) noexcept {
        if (this == &other)
            return *this;
        node = other.node;
        other.node = nullptr;
        return *this;
    }

    SceneNode *operator->() const;

    SceneNode &operator*() const;

    [[nodiscard]] bool isValid() const;

private:
    SceneNode *node;
};
