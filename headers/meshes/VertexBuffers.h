#pragma once
#include <cstdint>

// Describes a buffer for a single vertex binding
class VertexBindingBuffer {
public:
    VertexBindingBuffer() = default;

    VertexBindingBuffer(uint64_t dataSize, uint32_t elementSize) : dataSize(dataSize), elementSize(elementSize) {
        data = ::operator new(dataSize);
    }

    ~VertexBindingBuffer() {
        if (data != nullptr) {
            ::operator delete(data);
            data = nullptr;
        }
    }

    void setDataSize(const uint64_t dataSize) {
        this->dataSize = dataSize;
        if (data != nullptr) {
            ::operator delete(data);
            data = nullptr;
        }

        data = ::operator new(dataSize);
    }

    [[nodiscard]] uint64_t getDataSize() const { return dataSize; }

    [[nodiscard]] void *getData() const { return data; }

private:
    void *data{nullptr};
    uint64_t dataSize{0};
    uint64_t elementSize{0};
};

// Describes raw data for the whole vertex buffer on the CPU
class VertexBuffer {
public:
    VertexBuffer() = default;

    void setDataSize(const uint32_t binding, const uint64_t dataSize) {
        if (binding >= bindings.size())
            throw std::invalid_argument("Invalid binding index!");

        bindings[binding].setDataSize(dataSize);
    }

    [[nodiscard]] void *getData(const uint32_t binding) const {
        if (binding >= bindings.size())
            throw std::invalid_argument("Invalid binding index!");
        return bindings[binding].getData();
    }

    [[nodiscard]] uint64_t getDataSize(const uint32_t binding) const { return bindings[binding].getDataSize(); }

private:
    std::array<VertexBindingBuffer, SolVK::maxBindings> bindings{};
};
