#include <iostream>

#include "RenderingEngine.h"
#include "Scene.h"
#include "SceneNodeDescriptor.h"
#include "SceneNode.h"

bool runTests();

int main() {
    if (!runTests()) return 1;

    const auto engine = std::make_unique<RenderingEngine>();

    engine->initialize();
    engine->exec();

    return 0;
}

bool runTests() {
    auto success = true;

    Scene scene{};
    const auto root = scene.addNode(SceneNodeDescriptor{
        .name = "Test Root", .transform = Transform(glm::vec3(1.0f, 2.0f, 3.0f))
    });
    root->addChild(SceneNodeDescriptor{.name = "Child1", .transform = Transform(glm::vec3(1.0f, 0.0f, 0.0f))});
    root->addChild(SceneNodeDescriptor{.name = "Child2", .transform = Transform(glm::vec3(-1.0f, -2.0f, -3.0f))});
    const auto child = root->addChild(SceneNodeDescriptor{
        .name = "Child3", .transform = Transform{glm::vec3(-1.0f, -1.0f, -1.0f)}
    });
    auto lv2child = child->addChild({.name = "Lv2child", .transform = {}});

    scene.addNode({.name = "Other root", .transform = {}});

    lv2child->setLocalPosition(glm::vec3(10.0f, 10.0f, 10.0f));

    auto lv2childGlobalPos = lv2child->getGlobalPosition();
    if (lv2childGlobalPos != glm::vec3(10.0f, 11.0f, 12.0f)) {
        std::cout << "Position mismatch, got (" << lv2childGlobalPos.x << ", " << lv2childGlobalPos.y << ", " <<
                lv2childGlobalPos.z << "), expected (10, 11, 12)" << std::endl;
        success = false;
    }

    child->setLocalPosition(glm::vec3(123.0f, 0.0f, 0.0f));

    lv2childGlobalPos = lv2child->getGlobalPosition();
    if (lv2childGlobalPos != glm::vec3(134.0f, 12.0f, 13.0f)) {
        std::cout << "Position mismatch, got (" << lv2childGlobalPos.x << ", " << lv2childGlobalPos.y << ", " <<
                lv2childGlobalPos.z << "), expected (134, 12, 13)" << std::endl;
        success = false;
    }

    scene.logScene();

    return success;
}
