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
    const auto root = scene.addNode(SceneNodeDescriptor{.name = "Test Root", .transform = {}});
    root->addChild(SceneNodeDescriptor{.name = "Child1", .transform = {}});
    root->addChild(SceneNodeDescriptor{.name = "Child2", .transform = {}});
    const auto child = root->addChild(SceneNodeDescriptor{.name = "Child3", .transform = {}});
    child->addChild({.name = "Lv2child", .transform = {}});

    scene.addNode({.name = "Other root", .transform = {}});

    scene.logScene();

    return true;
}
