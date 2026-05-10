#include "Scene.h"

#include <stack>
#include <iostream>

void Scene::logScene() {
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
