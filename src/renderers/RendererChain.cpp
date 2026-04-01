#include "RendererChain.h"

#include <iostream>
#include <ostream>

void RendererChain::logState() const {
    std::cout << "==========================================" << std::endl;
    std::cout << "Renderer Chain:" << std::endl;
    std::cout << "==========================================" << std::endl;
    for (auto &&renderer: renderers) {
        std::cout << renderer->getInfo() << std::endl;
        std::cout << "==========================================" << std::endl;
    }
}
