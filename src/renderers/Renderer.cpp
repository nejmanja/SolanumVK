#include "Renderer.h"

#include <iostream>
#include <ostream>


void Renderer::setInputImages(std::vector<ImageResource *> &inputs) {
    inputImages = std::move(inputs);
    if (inputImages.size() != numInputImages) {
        std::cout << "WARNING: Failed to provide appropriate amount of inputs to renderer." << std::endl
                << "Expected inputs:" << std::endl;
        for (const auto &description: inputDescriptions) {
            std::cout << description.description << std::endl;
        }
    }
}

void Renderer::setOutputImages(std::vector<ImageResource *> &outputs) {
    outputImages = std::move(outputs);
    if (outputImages.size() != numOutputImages) {
        std::cout << "WARNING: Failed to provide appropriate amount of outputs to renderer." << std::endl
                << "Expected outputs:" << std::endl;
        for (const auto &description: outputDescriptions) {
            std::cout << description.description << std::endl;
        }
    }
}
