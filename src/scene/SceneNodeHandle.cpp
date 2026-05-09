#include "SceneNodeHandle.h"

#include "SceneNode.h"

SceneNode *SceneNodeHandle::operator->() const {
    return node;
}

SceneNode &SceneNodeHandle::operator*() const {
    return *node;
}

bool SceneNodeHandle::isValid() const {
    return node != nullptr;
}
