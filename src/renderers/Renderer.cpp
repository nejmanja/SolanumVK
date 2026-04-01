#include "Renderer.h"

#include <iostream>
#include <ostream>
#include <sstream>


std::string Renderer::getInfo() const {
    std::ostringstream stream{};

    stream << "Renderer [" << name << "]" << std::endl;

    if (!inputDescriptions.empty()) {
        stream << "INPUTS:" << std::endl;
        for (auto &&inputDescription: inputDescriptions) {
            stream << "\t" << inputDescription.description << std::endl;
        }
    }

    if (!outputDescriptions.empty()) {
        stream << "OUTPUTS:" << std::endl;
        for (auto &&outputDescription: outputDescriptions) {
            stream << "\t" << outputDescription.description << std::endl;
        }
    }

    return stream.str();
}

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
