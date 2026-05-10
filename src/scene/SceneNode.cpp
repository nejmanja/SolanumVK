#include "SceneNode.h"

#include <sstream>
#include <stack>

std::string SceneNode::toString() {
    if (globalTransformStale) recalculateGlobalTransform();

    std::ostringstream ss{};

    ss << name << std::endl;
    ss << "Transform:" << std::endl << "\tLocal: ";
    const auto localTranslation = localTransform.getTranslation();
    ss << "Pos (" << localTranslation.x << ", " << localTranslation.y << ", " << localTranslation.z << ")" <<
            std::endl;
    ss << "\tGlobal: ";
    const auto globalTranslation = globalTransform.getTranslation();
    ss << "Pos (" << globalTranslation.x << ", " << globalTranslation.y << ", " << globalTranslation.z << ")" <<
            std::endl;

    return ss.str();
}

void SceneNode::setLocalPosition(const glm::vec3 localPosition) {
    localTransform.setTranslation(localPosition);

    if (parent != nullptr) {
        globalTransform.setTransformMatrix(
            parent->globalTransform.getTransformMatrix() * localTransform.getTransformMatrix());
    } else {
        globalTransform.setTransformMatrix(localTransform.getTransformMatrix());
    }

    markChildrenGlobalTransformsStale();
}

void SceneNode::markChildrenGlobalTransformsStale() const {
    // Simple preorder traversal, mark all childern as dirty
    std::stack<SceneNode *> childStack{};
    for (auto &&child: children) {
        childStack.push(child.get());
    }

    while (!childStack.empty()) {
        SceneNode *child = childStack.top();
        childStack.pop();
        child->globalTransformStale = true;

        for (auto &&subChild: child->children) {
            childStack.push(subChild.get());
        }
    }
}

void SceneNode::recalculateGlobalTransform() {
    // Traverse up the tree to the first clean node, multiplying local transforms along the way
    auto *parentNode = parent;
    glm::mat4 transformMatrix = localTransform.getTransformMatrix();
    while (parentNode != nullptr && parentNode->globalTransformStale) {
        transformMatrix = parentNode->localTransform.getTransformMatrix() * transformMatrix;
        parentNode = parentNode->parent;
    }

    // the last gathered node is either null or clean
    if (parentNode != nullptr) // if it's null, the whole chain is done.
        // otherwise, just multiply with the global transform, since that one is clean
        transformMatrix = parentNode->globalTransform.getTransformMatrix() * transformMatrix;

    globalTransform.setTransformMatrix(transformMatrix);
    globalTransformStale = false;
}
