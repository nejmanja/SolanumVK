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

    scene.logScene();

    lv2child->setLocalPosition(glm::vec3(10.0f, 10.0f, 10.0f));

    scene.logScene();

    child->setLocalPosition(glm::vec3(123.0f, 0.0f, 0.0f));

    scene.logScene();

    return true;
}
