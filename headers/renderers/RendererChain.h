#pragma once
#include "Renderer.h"

class RendererChain {
public:
    RendererChain() = default;

    template<typename T>
    void addRenderer(std::unique_ptr<T> &renderer) {
        static_assert(std::is_base_of_v<Renderer, T>, "T must derive from Renderer!");
        renderers.push_back(std::move(renderer));
    }

    void prepareFrame(const double deltaTime) const {
        for (auto &&renderer: renderers) {
            renderer->prepareFrame(deltaTime);
        }
    }

    void execute(CommandManager &cmd) const {
        for (auto &&renderer: renderers) {
            renderer->execute(cmd);
        }
    }

    void logState() const;

private:
    // Renderers - execute in-order of the vector!
    std::vector<std::unique_ptr<Renderer> > renderers{};
};
