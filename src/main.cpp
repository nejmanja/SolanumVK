#include "RenderingEngine.h"

int main() {
    std::unique_ptr<RenderingEngine> engine = std::make_unique<RenderingEngine>();

    engine->initialize();
    engine->exec();

    return 0;
}
