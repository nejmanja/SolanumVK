#include "Scene.h"

#include <stack>
#include <iostream>

void Scene::logScene() const {
    for (const auto &node: rootObjects) {
        logRootObjectTree(node.get());
    }
}

void Scene::logRootObjectTree(SceneNode *root) {
    std::stack<SceneNode *> stack{};
    std::stack<std::string> prefixes{};
    stack.push(root);
    prefixes.emplace("");

    while (!stack.empty()) {
        auto *node = stack.top();
        auto prefix = prefixes.top();
        stack.pop();
        prefixes.pop();

        std::cout << prefix << node->getName() << std::endl;
        std::cout << '\t' << node->toString() << std::endl;

        for (auto &child: node->getChildren()) {
            prefixes.push(prefix + node->getName() + '/');
            stack.push(child.get());
        }
    }
}

void Scene::rebuildRenderList() {
    renderList.clear();

    std::stack<SceneNode *> nodeStack{};

    for (const auto &rootObject: rootObjects) {
        nodeStack.push(rootObject.get());
    }

    while (!nodeStack.empty()) {
        auto node = nodeStack.top();
        nodeStack.pop();
        if (node->getMeshData().has_value()) {
            renderList.addNode(SceneNodeHandle{node});
        }

        for (auto &&child: node->getChildren()) {
            nodeStack.push(child.get());
        }
    }
}
