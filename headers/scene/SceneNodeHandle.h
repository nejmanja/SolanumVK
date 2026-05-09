#pragma once

// Fwd declaration to prevent circular dependency w/ SceneNode
class SceneNode;

class SceneNodeHandle {
public:
    explicit SceneNodeHandle(SceneNode *node) : node(node) {
    }

    SceneNode *operator->() const;

    SceneNode &operator*() const;

    [[nodiscard]] bool isValid() const;

private:
    SceneNode *node;
};
